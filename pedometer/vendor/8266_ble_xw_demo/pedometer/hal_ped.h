
#ifndef HAL_PED_H
#define HAL_PED_H

/* ------------------------------------------------------------------------------------------------
 *                                       Global Functions
 * ------------------------------------------------------------------------------------------------
 */
void HalPedInit(void);
//void accUpdatePed(float x, float y, float z);
void HalaccUpdatePed(unsigned char *buf, unsigned int length);
void accResetPed(void);
void accSetSteps(int s);
int accGetSteps(void);
int accGetTest(void);

#endif
