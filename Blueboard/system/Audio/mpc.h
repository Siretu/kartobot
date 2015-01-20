/*
 * mpc.h
 *
 *  Created on: 18 Jul 2012
 *      Author: exuvo
 */

#ifndef MPC_H_
#define MPC_H_



#ifdef __cplusplus
extern "C" {
#endif

void Audio_MPC_Setup();
int16_t Audio_MPC_Play();
void Audio_MPC_Cleanup();

#ifdef __cplusplus
}
#endif
#endif /* MPC_H_ */
