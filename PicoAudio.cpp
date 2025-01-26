#include <cstring>

#include "Audio.hpp"

namespace AudioHandler {
	static bool ch0_is_coin = false;

	void audio_callback(blit::AudioChannel &) {
		using namespace blit;

		// setting wave_buffer_offset to 63 would make this run every sample

		// jump sweep
		if(channels[1].adsr_phase != ADSRPhase::OFF) {
			auto jump_sample = channels[1].adsr_frame;
			if(channels[1].adsr_phase == ADSRPhase::DECAY)
				jump_sample += (channels[1].attack_ms * sample_rate) / 1000;

			const int jump_len_samples = sample_rate * 0.2f;

			channels[1].frequency = 512 + jump_sample * 512 / jump_len_samples;
		} else
			channels[1].frequency = 512; // reset

		// coin
		const uint32_t coin_freq_change = 0xFFFFFF * 0.66f; // when we've decayed below this

		if(ch0_is_coin)
			channels[0].frequency = channels[0].adsr > coin_freq_change ? 800 : 1280;
		channels[2].frequency = channels[2].adsr > coin_freq_change ? 932 : 1396;

		// enemy death
		//channels[3].frequency = 300 - (channels[3].adsr >> 16);
		// enemy hurt
		//channels[4].frequency = 300 - (channels[1].adsr >> 16);

		// player death
		channels[5].frequency = 500 - (channels[5].adsr >> 16);

		// enemy throw
		channels[6].frequency = 450 - (channels[6].adsr >> 16) / 2;
	}

	AudioHandler::AudioHandler() {
	}

	void AudioHandler::init() {
		// Coin A / Select
		blit::channels[0].waveforms = blit::Waveform::TRIANGLE;
		blit::channels[0].frequency = 400;
		blit::channels[0].attack_ms = 30;
		blit::channels[0].decay_ms = 50;
		blit::channels[0].sustain = 0;
		blit::channels[0].release_ms = 0;

		// Jump
		blit::channels[1].waveforms = blit::Waveform::TRIANGLE; // kinda
		blit::channels[1].frequency = 440;
		blit::channels[1].attack_ms = 50;
		blit::channels[1].decay_ms = 150;
		blit::channels[1].sustain = 0;
		blit::channels[1].release_ms = 0;

		// Coin B
		blit::channels[2].waveforms = blit::Waveform::TRIANGLE;
		blit::channels[2].frequency = 932;
		blit::channels[2].attack_ms = 1;
		blit::channels[2].decay_ms = 199;
		blit::channels[2].sustain = 0;
		blit::channels[2].release_ms = 0;

		// Enemy Death
		blit::channels[3].waveforms = blit::Waveform::SQUARE;
		blit::channels[3].frequency = 300;
		blit::channels[3].attack_ms = 100;
		blit::channels[3].decay_ms = 100;
		blit::channels[3].sustain = 0;
		blit::channels[3].release_ms = 0;

		// Enemy Injured
		blit::channels[4].waveforms = blit::Waveform::SQUARE;
		blit::channels[4].frequency = 300;
		blit::channels[4].attack_ms = 100;
		blit::channels[4].decay_ms = 100;
		blit::channels[4].sustain = 0;
		blit::channels[4].release_ms = 0;

		// Player Death
		blit::channels[5].waveforms = blit::Waveform::SQUARE;
		blit::channels[5].frequency = 500;
		blit::channels[5].attack_ms = 200;
		blit::channels[5].decay_ms = 20;
		blit::channels[5].sustain = 0;
		blit::channels[5].release_ms = 0;

		// Enemy Throw
		blit::channels[6].waveforms = blit::Waveform::SQUARE;
		blit::channels[6].frequency = 450;
		blit::channels[6].attack_ms = 150;
		blit::channels[6].decay_ms = 20;
		blit::channels[6].sustain = 0;
		blit::channels[6].release_ms = 0;

		// Pico Tune!
		blit::channels[7].waveforms = blit::Waveform::SQUARE;
		blit::channels[7].frequency = 450;
		blit::channels[7].attack_ms = 110;
		blit::channels[7].decay_ms = 5;
		blit::channels[7].sustain = 0;
		blit::channels[7].release_ms = 0;

		blit::channels[7].wave_buffer_callback = audio_callback;
		memset(blit::channels[7].wave_buffer, 0, sizeof(blit::channels[7].wave_buffer));

#ifdef BLIT_BOARD_PIMORONI_PICOSYSTEM
		// PicoSystem can only do square
		for(int i = 0; i < CHANNEL_COUNT; i++)
			blit::channels[i].waveforms = blit::Waveform::SQUARE;
#endif
	}

	void AudioHandler::set_volume(uint32_t volume) {
		for (uint8_t i = 0; i < 8; i++) {
			set_volume(i, volume);
		}
	}

	void AudioHandler::set_volume(uint8_t channel, uint32_t volume) {
		blit::channels[channel].volume = volume;
	}

	void AudioHandler::load(uint8_t channel, const uint8_t mp3_data[], const uint32_t mp3_size) {
		(void)channel;
		(void)mp3_data;
		(void)mp3_size;
	}

	void AudioHandler::load(uint8_t target_channel, uint8_t source_channel) {
		(void)target_channel;
		(void)source_channel;

		if(target_channel == 0) {
			if(source_channel == 0) {
				// switching to select sound
				blit::channels[0].frequency = 400;
				blit::channels[0].attack_ms = 30;
				blit::channels[0].decay_ms = 50;
				ch0_is_coin = false;
			} else if(source_channel == 2) {
				// switching to coin sound
				blit::channels[0].frequency = 800;
				blit::channels[0].attack_ms = 1;
				blit::channels[0].decay_ms = 199;
				ch0_is_coin = true;
			}
		}
	}

	void AudioHandler::play(uint8_t channel, uint8_t flags) {
		if (channel <= 6 && blit::channels[channel].volume) {
			blit::channels[channel].trigger_attack();
		} else if (channel == 7 && blit::channels[7].volume) {
			// Play a tune!
			blit::channels[7].waveforms = blit::Waveform::SQUARE;
			blit::channels[7].sustain = 0;
			play_tune = true;
			note = 0;
			t = 0.0f;
		}
	}

	bool AudioHandler::is_playing(uint8_t channel) {
		// Requires player to press A for splash screen to end
		return true;
	}

	void AudioHandler::update(float dt) {
		using namespace blit;

		if (play_tune) {
			t += dt;
			if (t > 0.125f) {
				t = 0.0f;
				if (tune[note] != 0) {
					channels[7].frequency = std::pow(2, (tune[note] - 69) / 12.0f) * 440;
					channels[7].trigger_attack();
				}
				note++;
				if (note == 24) {
					play_tune = false;
				}
			}
		} else if(!play_tune && channels[7].adsr_phase == ADSRPhase::OFF) {
			// re-purpose the music channel as an audio timer when not playing the tune
			blit::channels[7].waveforms = blit::Waveform::WAVE;
			blit::channels[7].sustain = 0xFFFF;
			blit::channels[7].trigger_sustain();
		}
	}
}
