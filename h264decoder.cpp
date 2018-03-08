/*
 * h264decoder.cpp -- decode h264.
 * Copyright © 2015  e-con Systems India Pvt. Limited
 *
 * This file is part of Qtcam.
 *
 * Qtcam is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Qtcam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qtcam. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPainter>
#include "h264decoder.h"

using namespace std;


H264Decoder::H264Decoder()
{
    initVars();
    av_register_all();    
}

H264Decoder::~H264Decoder()
{
    closeFile();
}

bool H264Decoder::initH264Decoder(unsigned width, unsigned height)
{
    // find the decoder
#if !LIBAVCODEC_VER_AT_LEAST(54, 25)
    m_pH264Codec = avcodec_find_decoder(CODEC_ID_H264);
#else
    m_pH264Codec = avcodec_find_decoder(AV_CODEC_ID_H264);
#endif
    if (!m_pH264Codec)
    {        
        return false;
    }

#if LIBAVCODEC_VER_AT_LEAST(53,6)
    m_pH264CodecCtx = avcodec_alloc_context3(m_pH264Codec);
    avcodec_get_context_defaults3 (m_pH264CodecCtx, m_pH264Codec);
#else
    m_pH264CodecCtx = avcodec_alloc_context();
    avcodec_get_context_defaults(m_pH264CodecCtx);
#endif

    m_pH264CodecCtx->flags2 |= CODEC_FLAG2_FAST;

#if !LIBAVCODEC_VER_AT_LEAST(54, 25)
    m_pH264CodecCtx->pix_fmt = PIX_FMT_YUV420P;
#else
    m_pH264CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
#endif

    m_pH264CodecCtx->width = width;
    m_pH264CodecCtx->height = height;

#if LIBAVCODEC_VER_AT_LEAST(53,6)
    if (avcodec_open2(m_pH264CodecCtx, m_pH264Codec, NULL) < 0)
#else
    if (avcodec_open(m_pH264CodecCtx, m_pH264Codec) < 0)
#endif
    {        
        return false;
    }

#if LIBAVCODEC_VER_AT_LEAST(55,28)
    m_pH264picture = av_frame_alloc();
#else
    m_pH264picture = avcodec_alloc_frame();
#endif
    if(m_pH264picture == 0)
        return false;

#if LIBAVCODEC_VER_AT_LEAST(55,28)
    av_frame_unref(m_pH264picture);
#else
    avcodec_get_frame_defaults(m_pH264picture);
#endif

    m_h264PictureSize = avpicture_get_size(m_pH264CodecCtx->pix_fmt, m_pH264CodecCtx->width, m_pH264CodecCtx->height);
    m_h264pictureBuf = new uint8_t[m_h264PictureSize];
    if(m_h264pictureBuf == 0)
    {
        av_free(m_pH264picture);
        m_pH264picture = 0;
        return false;
    }    
    return true;
}


int H264Decoder::decodeH264(u_int8_t *outBuf, u_int8_t *inBuf, int bufSize)
{
    AVPacket avpkt;
    avpkt.size = bufSize;
    avpkt.data = inBuf;

    int gotPicture = 0;

    int len = avcodec_decode_video2(m_pH264CodecCtx, m_pH264picture, &gotPicture, &avpkt);
    if (len < 0)
        return len;

    if (gotPicture) {
        avpicture_layout((AVPicture *) m_pH264picture, m_pH264CodecCtx->pix_fmt
                         ,m_pH264CodecCtx->width, m_pH264CodecCtx->height, outBuf, m_h264PictureSize);
        return len;
    } else {
        return 0;
    }
}


void H264Decoder::closeFile()
{
    avcodec_close(m_pH264CodecCtx);
    freeFrame();
}


void H264Decoder::initVars()
{
    m_pH264CodecCtx = 0;
    m_pH264Codec = 0;
    m_pH264picture = 0;
}


void H264Decoder::freeFrame()
{
    if(m_h264pictureBuf) {
        delete[] m_h264pictureBuf;
        m_h264pictureBuf = 0;
    }

    if(m_pH264picture) {
        av_free(m_pH264picture);
        m_pH264picture = 0;
    }
}
