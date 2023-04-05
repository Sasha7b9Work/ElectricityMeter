﻿// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


void HAL_Init();
void HAL_ErrorHandler();


// num_sector от 0 до 127. Каждый сектор занимает 2 кБ
void HAL_ROM_EraseSector(int num_secttor);


#ifdef __cplusplus
extern "C" {
#endif 

    /* function declarations */
    /* this function handles NMI exception */
    void NMI_Handler(void);
    /* this function handles HardFault exception */
    void HardFault_Handler(void);
    /* this function handles MemManage exception */
    void MemManage_Handler(void);
    /* this function handles BusFault exception */
    void BusFault_Handler(void);
    /* this function handles UsageFault exception */
    void UsageFault_Handler(void);
    /* this function handles SVC exception */
    void SVC_Handler(void);
    /* this function handles DebugMon exception */
    void DebugMon_Handler(void);
    /* this function handles PendSV exception */
    void PendSV_Handler(void);
    /* this function handles SysTick exception */
    void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif
