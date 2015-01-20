/*
 * DiskImg.h
 *
 *  Created on: 24 Jun 2012
 *      Author: exuvo
 */

#ifndef DISKIMG_H_
#define DISKIMG_H_

/* MSC Disk Image Definitions */
#define MSC_ImageSize   0x00001000

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t DiskImage[MSC_ImageSize];   /* Disk Image */

#ifdef __cplusplus
}
#endif

#endif /* DISKIMG_H_ */
