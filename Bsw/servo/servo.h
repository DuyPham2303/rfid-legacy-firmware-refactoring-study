#ifndef INC_SERVO_H_
#define INC_SERVO_H_

typedef enum
{
    IN_RANGE,  /* chua quay het bien do */
    FULL_RANGE /* quay het bien do */
} ServoStatus_e;

void Servo_Init();
void Led_Init();

const ServoStatus_e Increase_ServoPos();
const ServoStatus_e Decrease_ServoPos();

void LedTimerInterruptEnable();
void LedTimerInterruptDisable();

#endif /* INC_SERVO_H_ */
