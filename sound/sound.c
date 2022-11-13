#include <sound/sound.h>

#define MINIAUDIO_IMPLEMENTATION
#include <third-party/miniaudio/miniaudio.h>




void
SND_AudioCallback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
    ma_decoder* decoder = (ma_decoder*)device->pUserData;
    if (decoder == 0)
        TORN_Log("SND: Miniaudio: Failed to decode!\n");
    
    ma_decoder_read_pcm_frames(decoder, output, frame_count, 0);
    
    (void)input;
}


torn_function SND_Sound* 
SND_CreateSound(u8* path)
{
    SND_Sound* sound = MEM_ArenaAlloc(MEM_GetDefaultArena(), sizeof(SND_Sound));
    
    sound->path = path;
    
    sound->result = ma_decoder_init_file(path, 0, &sound->decoder);
    if (sound->result != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Failed to decode file.\n");
        return 0;
    }
    
    
    ma_device_config device_config = {0};
    device_config = ma_device_config_init(ma_device_type_playback);
    device_config.playback.format =   sound->decoder.outputFormat;
    device_config.playback.channels = sound->decoder.outputChannels;
    device_config.sampleRate =        sound->decoder.outputSampleRate;
    device_config.dataCallback =      SND_AudioCallback;
    device_config.pUserData =         &sound->decoder;
    
    if (ma_device_init(0, &device_config, &sound->device) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Failed to open playback device.\n");
        ma_decoder_uninit(&sound->decoder);
        return 0;
    }
    
    ma_uint64 length;
    ma_decoder_get_length_in_pcm_frames(&sound->decoder, &length);
    sound->duration = length * 1000u / sound->decoder.outputSampleRate;
    
    TORN_Log("SND: Miniaudio: Created sound object \"%s\"\n", path);
    return sound;
    
}


torn_function void
SND_ResumeSound(SND_Sound* sound)
{
    sound->flag = SND_AudioPlaying;
    SND_SetSoundTime(sound, sound->sound_paused_at);
    if (ma_device_start(&sound->device) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Can't start playback device [[\"%s\"]]\n", sound->path);
        ma_device_uninit(&sound->device);
        ma_decoder_uninit(&sound->decoder);
    }
    
    TORN_Log("SND: Miniaudio: Playing audio.\n");
    
}
torn_function void
SND_PlaySound(SND_Sound* sound)
{
    sound->flag = SND_AudioPlaying;
    SND_SetSoundTime(sound, 0);
    if (ma_device_start(&sound->device) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Can't start playback device [[\"%s\"]]\n", sound->path);
        ma_device_uninit(&sound->device);
        ma_decoder_uninit(&sound->decoder);
    }
    
    TORN_Log("SND: Miniaudio: Playing audio.\n");
    
}

torn_function void
SND_SetSoundTime(SND_Sound* sound, i32 position)
{
    position = position / 1000u * sound->decoder.outputSampleRate;
    if (ma_decoder_seek_to_pcm_frame(&sound->decoder, position))
    {
        TORN_Log("SND: Miniaudio: Could not change the time position.\n");
    }
}

torn_function r32
SND_GetElapsedSoundTime(SND_Sound* sound)
{
    ma_uint64 time;
    if (ma_decoder_get_cursor_in_pcm_frames(&sound->decoder,
                                            &time) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Could not get the time elapsed.\n");
        
    }
    return time * 1000u / sound->decoder.outputSampleRate;
}


torn_function r32
SND_GetSoundVolume(SND_Sound* sound)
{
    r32 volume = 0;
    
    if (ma_device_get_master_volume(&sound->device, &volume) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Failed to get master volume.\n");
    }
    return volume*100;
}

torn_function void 
SND_SetSoundVolume(SND_Sound* sound, r32 volume)
{
    if (ma_device_set_master_volume(&sound->device, volume/100) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Could not set audio volume.\n");
    }
    
    TORN_Log("SND: Miniaudio: Set sound volume to %f.\n",volume);
}

torn_function void
SND_PauseSound(SND_Sound* sound)
{
    sound->sound_paused_at = SND_GetElapsedSoundTime(sound);
    sound->flag = SND_AudioPaused;
    if (ma_device_stop(&sound->device) != MA_SUCCESS)
    {
        TORN_Log("SND: Miniaudio: Failed to stop audio.\n");
        ma_device_uninit(&sound->device);
        ma_decoder_uninit(&sound->decoder);
    }
    TORN_Log("SND: Miniaudio: Stopped audio.\n");
}


torn_function SND_SoundFlag
SND_GetSoundState(SND_Sound* sound)
{
    if (sound->duration <= SND_GetElapsedSoundTime(sound))
    {
        sound->flag = SND_AudioStopped;
    }
    else if (sound->duration >= SND_GetElapsedSoundTime(sound) && (sound->flag != SND_AudioPaused))
    {
        sound->flag = SND_AudioPlaying;
    }
    
    return sound->flag;
}