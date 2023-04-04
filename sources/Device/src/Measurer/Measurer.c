// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"


static void Measurer_Calculate(void);

static struct FullMeasure measure;

static int measure_ready = 0;               // �� ����, ���� ��������� ������. ��������������� � 0 ����� ����������

static uint16 currentA[NUM_POINTS];
static uint16 currentB[NUM_POINTS];
static uint16 currentC[NUM_POINTS];

static uint16 voltA[NUM_POINTS];
static uint16 voltB[NUM_POINTS];
static uint16 voltC[NUM_POINTS];

static int16 pos_adc_value = 0;             // ������� ������� ����������� ��������


void Measurer_Update()
{
    if (Measurer_BuffersFull())
    {
        Measurer_Calculate();

        measure_ready = 1;
    }
}

void Measurer_AppendMeasures(uint16 adc_values[6])
{
    if (pos_adc_value < NUM_POINTS)
    {
        currentA[pos_adc_value] = adc_values[0];
        currentB[pos_adc_value] = adc_values[1];
        currentC[pos_adc_value] = adc_values[2];

        voltA[pos_adc_value] = adc_values[3];
        voltB[pos_adc_value] = adc_values[4];
        voltC[pos_adc_value] = adc_values[5];

        pos_adc_value++;
    }
}


int Measurer_BuffersFull()
{
    return pos_adc_value >= NUM_POINTS;
}


static void Measurer_Calculate()
{

}


int Measurer_MeasureReady()
{
    return measure_ready;
}


struct FullMeasure Measurer_GetMeasure()
{
    measure_ready = 0;

    return measure;
}
