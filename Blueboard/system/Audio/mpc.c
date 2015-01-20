/*
 * mpc.c
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#include "AudioInternal.h"
#include "mpc.h"
#include "mpc/include/mpc/reader.h"
#include "mpc/mpcdec.h"
#include "mpc/libmpcdec/decoder.h"
#include "ff.h"
#include "ff_test_term.h"
#include "term_io.h"
#include "Time2.h"
#include <string.h>

typedef struct mpc_data_t {
	mpc_reader reader;
	mpc_demux *demux;
	mpc_streaminfo si;
	mpc_frame_info frame;
	uint32_t i;//Buffer index
	MPC_SAMPLE_FORMAT buffer[MPC_DECODER_BUFFER_LENGTH];
} mpc_data;


 /// Reads size bytes of data into buffer at ptr.
mpc_int32_t mpc_reader_read(mpc_reader *p_reader, void *ptr, mpc_int32_t size){
	UINT read;
	FRESULT res = f_read ((FIL*) p_reader->data, ptr, size, &read);
	if(res){
		put_rc(res);
		return MPC_STATUS_FAIL;
	}
	return read;
}

/// Seeks to byte position offset.
mpc_bool_t mpc_reader_seek(mpc_reader *p_reader, mpc_int32_t offset){
	FRESULT res = f_lseek ((FIL*) p_reader->data, offset);
	if(res){
		put_rc(res);
		return MPC_FALSE;
	}else{
		return MPC_TRUE;
	}
}

/// Returns the current byte offset in the stream.
mpc_int32_t mpc_reader_tell(mpc_reader *p_reader){
	return f_tell((FIL*) p_reader->data);
}

/// Returns the total length of the source stream, in by
mpc_int32_t mpc_reader_get_size(mpc_reader *p_reader){
	return f_size((FIL*) p_reader->data);
}

/// True if the stream is a seekable stream.
mpc_bool_t mpc_reader_canseek(mpc_reader *p_reader){
	return MPC_TRUE;
}

mpc_status mpc_reader_init(mpc_reader * p_reader, const TCHAR *path){
	p_reader->data = pvPortMalloc2(sizeof(FIL));
	if(p_reader->data == NULL){
		return MPC_STATUS_FAIL;
	}

	FRESULT res = f_open((FIL*) p_reader->data, path, FA_READ);
	if (res) {
		put_rc(res);
		return MPC_STATUS_FAIL;
	} else {
		p_reader->read     = mpc_reader_read;
		p_reader->seek     = mpc_reader_seek;
    p_reader->tell     = mpc_reader_tell;
    p_reader->get_size = mpc_reader_get_size;
    p_reader->canseek  = mpc_reader_canseek;
		return MPC_STATUS_OK;
	}
}

void mpc_reader_close(mpc_reader *p_reader){
	FIL* f = (FIL*) p_reader->data;
	if(f){
		f_close(f);
	}
}

void Audio_MPC_Setup(){
	resources = pvPortMalloc2(sizeof(mpc_data));

	if(resources == NULL){
		xputs("Audio: MPC failed to allocate resources.\n");
		return;
	}
	memset(resources, 0, sizeof(mpc_data));

	mpc_data *data = (mpc_data*) resources;

	const TCHAR *path	=(const TCHAR*) currentSong.data;
	if(mpc_reader_init(&data->reader, path)){
		xputs("Audio: MPC failed to initialise reader.\n");
		return;
	}

	data->demux = mpc_demux_init(&data->reader);
	if(data->demux == 0){
		xputs("Audio: MPC failed to initialise demux.\n");
		return;
	}

  mpc_demux_get_info(data->demux,  &data->si);
  Audio_SetFrequency(data->si.sample_freq);

  data->frame.buffer = data->buffer;

	playing = 1;
}

int16_t Audio_MPC_Play(){
	mpc_data *data = (mpc_data*) resources;

	if(data->i >= data->frame.samples){
		uint32_t t = Time_Millis();
		mpc_status err = mpc_demux_decode(data->demux, &data->frame);
		t = Time_Millis() - t;

		xprintf("Decoded frame in %lu us", t);

		if(data->frame.bits == -1){//End of stream
			playing = 0;
			return 0;
		}

		if(err != MPC_STATUS_OK){
			xputs("Audio: MPC error occured while decoding.\n");
			playing = 0;
			return 0;
		}

		data->i = 0;

#ifdef MPC_FIXED_POINT
		for(int i=0; i < MPC_DECODER_BUFFER_LENGTH; i++) {
			int32_t tmp = data->buffer[i] >> MPC_FIXED_POINT_FRACTPART;
			if (tmp > ((1 << 15) - 1)) tmp = ((1 << 15) - 1);
			if (tmp < -(1 << 15)) tmp = -(1 << 15);
			data->buffer[i] = tmp;
		}
#endif
	}

	int16_t ret;
	switch(data->si.channels){
		case 1:
			ret = data->buffer[data->i++];
			break;
		case 2:
			ret = (data->buffer[data->i] + data->buffer[data->i+1]) / 2;
			data->i += 2;
			break;
		default:{
//			uint32_t sum;
//			for(uint8_t i=0; i < data->si.channels; i++){
//				sum += data->sample_buffer[data->frame_pointer++];
//			}
//			ret = sum / data->si.channels;
			ret = (data->buffer[data->i] + data->buffer[data->i+1]) / 2;
			data->i += data->si.channels;
			break;
		}
	}
	return ret;

}

void Audio_MPC_Cleanup(){
	if(resources != NULL){
		mpc_data *data = (mpc_data *) resources;
		mpc_demux_exit(data->demux);
		mpc_reader_close(&data->reader);

		vPortFree2(resources);
		resources = NULL;
	}

	if(currentSong.data != NULL){
		vPortFree2(currentSong.data);
	}
}

