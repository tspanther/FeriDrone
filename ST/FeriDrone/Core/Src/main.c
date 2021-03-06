/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "vl53l0x_api.h"
#include "math.h"
#include "MadgwickAHRS.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TOFS_devID 0x29

#define HELIX_FREQ 0.2f
#define HELIX_WL 5.0f
#define HELIX_STEP 1000

// MOCK
#define MOCK_TRILATERATION 1
#define MOCK_NAGIB 0
#define MOCK_ALTITUDE 0
#define MOCK_PWM_GEN 0

#define SEND_TRILATERATION 0
#define SEND_NAGIB 1
#define SEND_ALTITUDE 1
#define SEND_PWM_RAW 1
#define SEND_PWM_GEN 1
#define SEND_PWM_OUTGOING 0
#define SEND_THRUST_AIRSIM 0
#define SEND_AUTOLANDER_DEBUG 1

#define COMM_USB 0
#define COMM_WIFI 1
#define COMM_MODE COMM_USB

#define WIFI_BUFFER_SIZE 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

I2S_HandleTypeDef hi2s2;
I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

osThreadId_t merjenjeNagibaHandle;
osThreadId_t trilateracijaHandle;
osThreadId_t pilotiranjeHandle;
osThreadId_t posiljanjeWifiHandle;
osThreadId_t transmitPWMHandle;
osThreadId_t altitudeMeasureHandle;
osSemaphoreId_t wifiBufferSemaphoreHandle;
/* USER CODE BEGIN PV */

// AUTOLANDER
// algorithm
#define HEIGHT_SAMPLING_FREQ 30.0f
#define HEIGHT_SAMPLING_INTE (1.0f / HEIGHT_SAMPLING_FREQ)
#define TRILATERATION_SAMPLING_FREQ HEIGHT_SAMPLING_FREQ
#define ROUGH_EST_SAMPLES_NUM 6 // ustreza reaction time-u nekje 0.35s
volatile uint8_t heightSample_idx = 0;
volatile float heightSamples[ROUGH_EST_SAMPLES_NUM];
volatile float velocities[ROUGH_EST_SAMPLES_NUM];

// schedule
volatile uint8_t autolanderCommencing = 0;
volatile uint8_t autolanderScheduleReady = 0;
volatile uint16_t ts = 0;
volatile uint16_t tsDecel = 0;
volatile uint16_t tsStopDecel = 0;

// drone & physics
#define DRONE_MASS 0.5f
#define G 9.81f
#define OBJECT_MAX_THRUST 11.06f
#define OBJECT_NEUTRAL_THRUST (DRONE_MASS * G)

// PWM
#define PWM_FREQ 50.0f
#define THROTTLE_CHANNEL 2
#define AUTOLANDER_CHANNEL 5
#define PWM_LOW 800
#define PWM_HIGH 2050
#define PWM_PAUSE_LOW 6000
#define PWM_PAUSE_HIGH 14000
#define PWM_PACKET_LENGTH 25836
#define PWM_PAUSE 517
#define PWM_MS 1292
volatile uint16_t PWM_paket_new[8];
volatile uint8_t PWM_paket_ready = 0;
volatile uint16_t PWM_generated[8];
volatile uint8_t PWM_generated_ready = 0;
volatile uint16_t delay[18];
volatile uint8_t delay_idx = 0;

// wifi
float wifiBuffer[WIFI_BUFFER_SIZE];
volatile uint8_t wifiBufferIdx = 0;

// usb -- debug
extern float height_from_simul;
extern uint8_t height_ready;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
void StartMerjenjeNagiba(void *argument);
void StartTrilateracija(void *argument);
void StartPilotiranje(void *argument);
void StartPosiljanjeWifi(void *argument);
void StartTransmitPWM(void *argument);
void StartAltitudeMeasure(void *argument);

/* USER CODE BEGIN PFP */
uint8_t spi1_beriRegister(uint8_t);
void spi1_beriRegistre(uint8_t, uint8_t*, uint8_t);
void spi1_pisiRegister(uint8_t, uint8_t);
void initL3GD20(void);
uint8_t i2c1_pisiRegister(uint8_t, uint8_t, uint8_t);
void i2c1_beriRegistre(uint8_t, uint8_t, uint8_t*, uint8_t, uint8_t);
void initLSM303DLHC(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void initEsp8622TcpClient(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * Packs data in 5 floats large packet and adds it to queue;
 * Packet: [header * 1, values * (1-4), nil * (0-3)]
 */
void addToWifiBuffer(float* data, uint8_t size, uint8_t* lastBufferIdx){
	uint8_t nil[4] = { 0x00, 0x00, 0x00, 0x00 };
	float nil_f;
	memcpy(&nil_f, &nil, sizeof(float));
	if (wifiBufferIdx > WIFI_BUFFER_SIZE - size - 1){
		for (int i = 0; i < size; i++){
			// overwrite previous measurement of this task if buffer full
			wifiBuffer[i + *lastBufferIdx] = *(data + i);
		}
		for (int i = size; i < 5; i++) {
			wifiBuffer[i + *lastBufferIdx] = nil_f;
		}
	} else {
		for (int i = 0; i < size; i++){
			wifiBuffer[i + wifiBufferIdx] = *(data + i);
		}
		for (int i = size; i < 5; i++) {
			wifiBuffer[i + *lastBufferIdx] = nil_f;
		}
		*lastBufferIdx = wifiBufferIdx;
		wifiBufferIdx += 5;
	}

	return;
}

void addPWMToWifiBuffer(uint16_t* data, float header, uint8_t* lastBufferIdx){
	if (wifiBufferIdx > WIFI_BUFFER_SIZE - 5){
		wifiBuffer[(*lastBufferIdx)] = header;
		memcpy(&wifiBuffer[(*lastBufferIdx) + 1], data, 8 * sizeof(uint16_t));
	} else {
		wifiBuffer[wifiBufferIdx] = header;
		memcpy(&wifiBuffer[wifiBufferIdx + 1], data, 8 * sizeof(uint16_t));
		*lastBufferIdx = wifiBufferIdx;
		wifiBufferIdx += 5;
	}
}

void init_autolander(void) {
	autolanderCommencing = 1;
	autolanderScheduleReady = 0;
	ts = 0;
	heightSample_idx = 0;
}

void terminate_autolander(void) {
	autolanderCommencing = 0;
	autolanderScheduleReady = 0;
	ts = 0;
	heightSample_idx = 0;
}

uint8_t isAutolandRequested(uint16_t* PWM){
	return *(PWM + AUTOLANDER_CHANNEL) > 1800;
}

void autolander_compileSchedule(void){
	float sVel = 0.0f;
	for (uint8_t i = 0; i < ROUGH_EST_SAMPLES_NUM; i++){
		sVel -= velocities[i] / ROUGH_EST_SAMPLES_NUM;
	}
	float sAlt = 0.0f;
	for (uint8_t i = 0; i < ROUGH_EST_SAMPLES_NUM; i++){
		float v_i = sVel - i * HEIGHT_SAMPLING_INTE / 2.0f * G;
		sAlt += (heightSamples[i] + v_i * HEIGHT_SAMPLING_INTE * i) / ROUGH_EST_SAMPLES_NUM;
	}

	float h = sAlt + sVel * sVel / 2.0f / G;
	float aimedAltitude = 0.25;
	if (aimedAltitude < h / 20.0f){
		aimedAltitude = h / 20.0f;
	}
	h -= aimedAltitude;

	float F_d = OBJECT_MAX_THRUST - DRONE_MASS * G;
	float a_d = F_d / DRONE_MASS;
	float x = G * h / (G + a_d);
	float h_allowTravel = h - x;

	float timeToStartDecel = sqrt(2.0f * h_allowTravel / G);
	float moveTimeStep = sVel / G / HEIGHT_SAMPLING_INTE;

	tsDecel = (int)(timeToStartDecel / HEIGHT_SAMPLING_INTE + moveTimeStep);

	float maxVelocity = timeToStartDecel * G;
	float decelTime = maxVelocity / a_d;

	tsStopDecel = (int)(tsDecel + decelTime / HEIGHT_SAMPLING_INTE) + 1;

	autolanderScheduleReady = 1;

	if (SEND_AUTOLANDER_DEBUG) {
		float header;
		char b[] = { 0xaa, 0xa3, 0xaa, 0xa3 };
		memcpy(&header, &b, sizeof(float));

		float debug[5] = { header, sVel, sAlt, tsDecel, tsStopDecel };

		if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK){
			uint8_t idx = 0; // if necessary (buffer full), just overwrite data on idx 0 - nothing else is more important to send
			addToWifiBuffer(debug, 5, &idx);
			osSemaphoreRelease(wifiBufferSemaphoreHandle);
		}
	}

	return;
}

void autolander_newMeasurement(float measurement){
	if (!autolanderCommencing || autolanderScheduleReady) {
		ts++;
		return;
	}

	if (heightSample_idx < ROUGH_EST_SAMPLES_NUM){
		heightSamples[heightSample_idx] = measurement;
	} else if (heightSample_idx < ROUGH_EST_SAMPLES_NUM * 2){
		float velocityStep = (ts - ROUGH_EST_SAMPLES_NUM / 2) * HEIGHT_SAMPLING_INTE * G;
		velocities[heightSample_idx - ROUGH_EST_SAMPLES_NUM] = (heightSamples[heightSample_idx - ROUGH_EST_SAMPLES_NUM] - measurement) / (HEIGHT_SAMPLING_INTE * ROUGH_EST_SAMPLES_NUM) - velocityStep;
	}

	heightSample_idx++;

	if (heightSample_idx == ROUGH_EST_SAMPLES_NUM * 2) {
		autolander_compileSchedule();
	}

	ts++;
	return;
}

void CompressBuffer(void) {
	/* todo: Aljaz
	 *
	 * stiskanje 6 tokov (roll, pitch, yaw, pos_{x,y,z})
	 * vsak tok stiskaj posebej (vseh 6)
	 *
	 * globalno si shrani podatek o zadnji vrednosti iz vsakega toka ( float lastValue[6] )
	 * 	- da, vsakic ko kompresiras nov buffer/batch, ne zacnes znova
	 *
	 * 	sparsaj buffer - ugotovi, za kateri paket gre (0xAAAB static castan v float za roll, pitch yaw in 0xAAAC za pos_{x,y,z})
	 * 	floatu, ki je header sledijo 3 floati, ki so vrednosti; isti vrstni red, kot so nasteti (r->p->y oz. x->y->z)
	 *
	 * lahko si zamislis svoj protokol, kako bos zapisal nazaj v buffer (recimo ne rabis pisat headerjev kot celih floatov nazaj v buffer)
	 * prek USB/Wifi bo sel del bufferja; of indexa 0 do indexa wifiBufferIdx
	 * 	- kompresirano vsebino zapisi na zacetek bufferja in nastavi index, do kod naj se poslje
	 *
	*/
}

VL53L0X_Error InitDevice(VL53L0X_Dev_t *pMyDevice) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  uint8_t VhvSettings, PhaseCal, isApertureSpads;
  uint32_t refSpadCount;

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_DataInit(pMyDevice);

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_StaticInit(pMyDevice);

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_PerformRefSpadManagement(pMyDevice, &refSpadCount, &isApertureSpads); // Device Initialization

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings, &PhaseCal);

  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);

  return Status;
}

uint8_t spi1_beriRegister(uint8_t reg) {
	uint16_t buf_out, buf_in;
	reg |= 0x80;
	buf_out = reg;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &buf_out, (uint8_t*) &buf_in, 2,
			2);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	return buf_in >> 8;
}

void spi1_pisiRegister(uint8_t reg, uint8_t vrednost) {
	uint16_t buf_out;
	buf_out = reg | (vrednost << 8);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &buf_out, 2, 2);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}

void spi1_beriRegistre(uint8_t reg, uint8_t *buffer, uint8_t velikost) {
	reg |= 0xC0;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &reg, 1, 10);
	HAL_SPI_Receive(&hspi1, buffer, velikost, velikost);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}

void initL3GD20() {
	uint8_t cip = spi1_beriRegister(0x0F);
	if (cip != 0xD4)
		for (;;)
			;

	spi1_pisiRegister(0x20, 0x0F);
}

uint8_t i2c1_pisiRegister(uint8_t naprava, uint8_t reg, uint8_t podatek) {
	naprava <<= 1;
	return HAL_I2C_Mem_Write(&hi2c1, naprava, reg, I2C_MEMADD_SIZE_8BIT,
			&podatek, 1, 10);
}

void i2c1_beriRegistre(uint8_t naprava, uint8_t reg, uint8_t *podatek,
		uint8_t dolzina, uint8_t suci) {
	if ((dolzina > 1) && (naprava == 0x19))
		reg |= 0x80;
	naprava <<= suci;
	HAL_I2C_Mem_Read(&hi2c1, naprava, reg, I2C_MEMADD_SIZE_8BIT, podatek,
			dolzina, dolzina);
}

void initLSM303DLHC() {
	HAL_Delay(10);
	i2c1_pisiRegister(0x19, 0x20, 0x57); // accel; 0101 speed (100hz), 0 low power enable (no), 111 enable axis xyz
	i2c1_pisiRegister(0x19, 0x23, 0x88); // accel; 1(7); output registers not updated until MSB and LSB reading, 1(3); high resolution
	i2c1_pisiRegister(0x3c, 0x00, 0x18); // magnet 75hz
}

void initEsp8622TcpClient(){
	/*
	 *	TCP client initialization. WARNING: server needs to be running first, otherwise client restart necessary.
	 *	COMMON ISSUE: ESP8622 module replying busy. Increase timeout or add a very small
	 *	delay between transmit and receive. Or restart.
	 *	It is also possible to wait in UART_receive, until the module reply 'OK'.
	 *	WARNING: The TCP client should send data at least every 4 seconds, otherwise it gets auto
	 *	disconnected.
	 *	Uart1 (PA15_TX ; PB7_RX)-> TCP server.
	 *	Content in comments kept for debug purposes.
	 */

	uint8_t prejetoSporocilo[255] = {};

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWMODE=1\r\n", 13, 1000); // Station mode.
	HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 1000);
	/*CDC_Transmit_FS(prejetoSporocilo, 255);
	memset(prejetoSporocilo, 0, 255);*/

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWLAP\r\n", 10, 1000); // Lists available APs.
	HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 1000);
	/*CDC_Transmit_FS(prejetoSporocilo, 255);
	memset(prejetoSporocilo, 0, 255);*/

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWJAP=\"server\",\"123456780\"\r\n", 33, 1000); // Connect to designated AP.
	HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 1000);
	/*CDC_Transmit_FS(prejetoSporocilo, 255);
	memset(prejetoSporocilo, 0, 255);*/

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWJAP?\r\n", 11, 1000); // Query APs info.
	HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 1000);
	/*CDC_Transmit_FS(prejetoSporocilo, 255);
	memset(prejetoSporocilo, 0, 255);*/

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPMUX=1\r\n", 13, 1000); // Enable multiple connections.
	HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 1000);
	/*CDC_Transmit_FS(prejetoSporocilo, 255);
	memset(prejetoSporocilo, 0, 255);*/

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSTART=0,\"TCP\",\"192.168.4.1\",333\r\n", 39, 1000); // Establish TCP connection.
	HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 1000);
	/*CDC_Transmit_FS(prejetoSporocilo, 255);
	memset(prejetoSporocilo, 0, 255);*/
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_I2C3_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  __HAL_I2C_ENABLE(&hi2c3);
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_Base_Start_IT(&htim3);
  /* USER CODE END 2 */

  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of wifiBufferSemaphore */
  const osSemaphoreAttr_t wifiBufferSemaphore_attributes = {
    .name = "wifiBufferSemaphore"
  };
  wifiBufferSemaphoreHandle = osSemaphoreNew(1, 1, &wifiBufferSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of merjenjeNagiba */
  const osThreadAttr_t merjenjeNagiba_attributes = {
    .name = "merjenjeNagiba",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512
  };
  merjenjeNagibaHandle = osThreadNew(StartMerjenjeNagiba, NULL, &merjenjeNagiba_attributes);

  /* definition and creation of trilateracija */
  const osThreadAttr_t trilateracija_attributes = {
    .name = "trilateracija",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512
  };
  trilateracijaHandle = osThreadNew(StartTrilateracija, NULL, &trilateracija_attributes);

  /* definition and creation of pilotiranje */
  const osThreadAttr_t pilotiranje_attributes = {
    .name = "pilotiranje",
    .priority = (osPriority_t) osPriorityAboveNormal,
    .stack_size = 512
  };
  pilotiranjeHandle = osThreadNew(StartPilotiranje, NULL, &pilotiranje_attributes);

  /* definition and creation of posiljanjeWifi */
  const osThreadAttr_t posiljanjeWifi_attributes = {
    .name = "posiljanjeWifi",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512
  };
  posiljanjeWifiHandle = osThreadNew(StartPosiljanjeWifi, NULL, &posiljanjeWifi_attributes);

  /* definition and creation of transmitPWM */
  const osThreadAttr_t transmitPWM_attributes = {
    .name = "transmitPWM",
    .priority = (osPriority_t) osPriorityHigh,
    .stack_size = 512
  };
  transmitPWMHandle = osThreadNew(StartTransmitPWM, NULL, &transmitPWM_attributes);

  /* definition and creation of altitudeMeasure */
  const osThreadAttr_t altitudeMeasure_attributes = {
    .name = "altitudeMeasure",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512
  };
  altitudeMeasureHandle = osThreadNew(StartAltitudeMeasure, NULL, &altitudeMeasure_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 200;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 5;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */
	/*
	 * todo: fix in gui so correct settings don't get overwritten on code generation
	 *
htim1.Instance = TIM1;
  htim1.Init.Prescaler = 64;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
  HAL_TIM_IC_Start(&htim1, TIM_CHANNEL_1);
	*/
  /* USER CODE END TIM1_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 64;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 64;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin 
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : DATA_Ready_Pin */
  GPIO_InitStruct.Pin = DATA_Ready_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DATA_Ready_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INT1_Pin INT2_Pin MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = INT1_Pin|INT2_Pin|MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin 
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin 
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartMerjenjeNagiba */
/**
 * @brief  Function implementing the merjenjeNagiba thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMerjenjeNagiba */
void StartMerjenjeNagiba(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */

	float rezultat[4];
	char b[] = { 0xaa, 0xab, 0xaa, 0xab };
	memcpy(&rezultat[0], &b, sizeof(float));
	uint8_t lastBufferIdx = 0;

	if (MOCK_NAGIB) {
		// delay
		float freq = HELIX_FREQ * HELIX_STEP;
		float delay = 1000.0f / freq;
		uint32_t ticksDelay = (uint32_t) (delay / 1);

		float eps = 0.0001;
		float len_deriv = sqrt(/*derivative_x * derivative_x + derivative_y * derivative_y ==*/1 + HELIX_WL * HELIX_WL);
		float derivative_z = HELIX_WL / len_deriv;

		// loop
		float t = 0.0f;
		int reverse = 1;
		for (;;){
			uint32_t lastTick = osKernelGetTickCount();

			float derivative_x = -sin(t) / len_deriv;
			float derivative_y = cos(t) / len_deriv;

			float pitch = asin(-derivative_y);
			float yaw = atan2(derivative_x, derivative_z);
			float roll = -sin(t) / 10.0f;

			rezultat[1] = roll;
			rezultat[2] = pitch;
			rezultat[3] = yaw;

			if (SEND_NAGIB) {
				if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK){
					addToWifiBuffer(rezultat, 4, &lastBufferIdx);
					osSemaphoreRelease(wifiBufferSemaphoreHandle);
				}
			}

			t += reverse * M_PI * 2 / freq;
			if (t <= eps || t >= M_PI * 2 - eps) {
				reverse *= -1;
			}

			osDelayUntil(lastTick + ticksDelay);
		}
	} else {
		// init
		__HAL_SPI_ENABLE(&hspi1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
		initL3GD20();

		__HAL_I2C_ENABLE(&hi2c1);
		initLSM303DLHC();

		// delay
		float freq = MADGWICK_FREQ;
		float delay = 1000.0f / freq;
		uint32_t ticksDelay = (uint32_t) (delay / 1);

		int16_t meritev[9];
		for (;;) {
			uint32_t lastTick = osKernelGetTickCount();

			spi1_beriRegistre(0x28, (uint8_t*) &meritev[0], 6); // gyros
			i2c1_beriRegistre(0x19, 0x28, (uint8_t*) &meritev[3], 6, 1); // accel
			i2c1_beriRegistre(0x3c, 0x03, (uint8_t*) &meritev[6], 6, 0); // magnet
			MadgwickAHRSupdate(-meritev[1] * 0.00875 * M_PI / 180, meritev[0] * 0.00875 * M_PI / 180, meritev[2] * 0.00875 * M_PI / 180,
					meritev[3], meritev[4], meritev[5],
					meritev[6], meritev[8], meritev[7]);

			float roll = atan2(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1 * q1 + q2 * q2)) * 180 / M_PI;
			float pitch = asin(2 * (q0 * q2 - q3 * q1)) * 180 / M_PI;
			float yaw = atan2(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2 * q2 + q3 * q3)) * 180 / M_PI;

			rezultat[1] = roll;
			rezultat[2] = pitch;
			rezultat[3] = yaw;

			if (SEND_NAGIB) {
				if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK){
					addToWifiBuffer(rezultat, 4, &lastBufferIdx);
					osSemaphoreRelease(wifiBufferSemaphoreHandle);
				}
			}

			osDelayUntil(lastTick + ticksDelay);
		}
	}
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_StartTrilateracija */
/**
 * @brief Function implementing the trilateracija thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTrilateracija */
void StartTrilateracija(void *argument)
{
  /* USER CODE BEGIN StartTrilateracija */

	float rezultat[4];
	char b[] = { 0xaa, 0xac, 0xaa, 0xac };
	memcpy(&rezultat[0], &b, sizeof(float));
	uint8_t lastBufferIdx = 0;

	if (MOCK_TRILATERATION) {
		// delay
		float freq = 25.0f;//HELIX_FREQ * HELIX_STEP / 2;
		float delay = 1000.0f / freq;
		uint32_t ticksDelay = (uint32_t) (delay / 1);

		float eps = 0.0001;

		// loop
		float t = 0.0f;
		int reverse = 1;
		for (;;) {
			uint32_t lastTick = osKernelGetTickCount();

			float pos_x = cos(t);
			float pos_y = sin(t);
			float pos_z = t;

			rezultat[1] = pos_x;
			rezultat[2] = pos_y;
			rezultat[3] = pos_z;

			if (SEND_TRILATERATION) {
				if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, ticksDelay) == osOK){
					addToWifiBuffer(rezultat, 4, &lastBufferIdx);
					osSemaphoreRelease(wifiBufferSemaphoreHandle);
				}
			}

			t += reverse * M_PI * 2 / freq;
			if (t <= eps || t >= M_PI * 2 - eps) {
				reverse *= -1;
			}

			osDelayUntil(lastTick + ticksDelay);
		}
	} else {
		for (;;) {
			osDelay(100);
		}
	}

  /* USER CODE END StartTrilateracija */
}

/* USER CODE BEGIN Header_StartPilotiranje */
/**
 * @brief Function implementing the pilotiranje thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPilotiranje */
void StartPilotiranje(void *argument)
{
  /* USER CODE BEGIN StartPilotiranje */
	uint8_t lastBufferIdx = 0;

	for (;;) {
		if (PWM_paket_ready) {
			uint16_t PWM[8];

			for (uint8_t i = 0; i < 8; i++) {
				// todo: samo menjaj kazalce
				PWM[i] = PWM_paket_new[i];
			}

			if (SEND_PWM_RAW) {
				float header;
				char b[] = { 0xaa, 0xae, 0xaa, 0xae };
				memcpy(&header, &b, sizeof(float));
				if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK){
					addPWMToWifiBuffer(&PWM[0], header, &lastBufferIdx);
					osSemaphoreRelease(wifiBufferSemaphoreHandle);
				}
			}

			uint8_t autolander = isAutolandRequested(&PWM[0]);
			if (autolander && !autolanderCommencing) {
				init_autolander();
			} else if (!autolander && autolanderCommencing){
				terminate_autolander();
			}

			if (autolanderCommencing) {
				if (autolanderScheduleReady) {
					if (ts < tsDecel) {
						PWM[THROTTLE_CHANNEL] = PWM_LOW;
					} else if (ts < tsStopDecel) {
						PWM[THROTTLE_CHANNEL] = PWM_HIGH;
					} else {
						PWM[THROTTLE_CHANNEL] = (int)(PWM_LOW + (float)(PWM_HIGH - PWM_LOW) * (OBJECT_NEUTRAL_THRUST * 0.90f / OBJECT_MAX_THRUST));
					}
				} else {
					PWM[THROTTLE_CHANNEL] = PWM_LOW;
				}
			}

			for (uint8_t i = 0; i < 8; i++) {
				// todo: samo menjaj kazalce
				PWM_generated[i] = PWM[i];
				PWM_generated_ready = 1;
			}

			if (SEND_PWM_GEN) {
				float header;
				char b[] = { 0xaa, 0xaf, 0xaa, 0xaf };
				memcpy(&header, &b, sizeof(float));
				if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK){
					addPWMToWifiBuffer(&PWM_generated[0], header, &lastBufferIdx);
					osSemaphoreRelease(wifiBufferSemaphoreHandle);
				}
			}

			PWM_paket_ready = 0;
		}
		osDelay(1);
	}
  /* USER CODE END StartPilotiranje */
}

/* USER CODE BEGIN Header_StartPosiljanjeWifi */
/**
* @brief Function implementing the posiljanjeWifi thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPosiljanjeWifi */
void StartPosiljanjeWifi(void *argument)
{
  /* USER CODE BEGIN StartPosiljanjeWifi */
	// delay
	float floatsPerSec = 0.0f
			+ SEND_ALTITUDE * HEIGHT_SAMPLING_FREQ * 5.0f
			+ SEND_NAGIB * MADGWICK_FREQ * 5.0f
			+ SEND_TRILATERATION * TRILATERATION_SAMPLING_FREQ * 5.0f
			+ (SEND_PWM_GEN + SEND_PWM_RAW + SEND_PWM_OUTGOING) * PWM_FREQ * 5.0f
			+ SEND_THRUST_AIRSIM * HEIGHT_SAMPLING_FREQ * 5.0f
			+ SEND_AUTOLANDER_DEBUG * 5.0f; // not sent each second, but important

	float freq = floatsPerSec / (WIFI_BUFFER_SIZE * 0.7f); // nekaj rezerve
	float delay = 1000.0f / freq;
	uint32_t ticksDelay = (uint32_t) (delay / 1);

  /* Infinite loop */
	if (COMM_MODE == COMM_USB) {
		for (;;) {
			uint32_t lastTick = osKernelGetTickCount();

			if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, ticksDelay) == osOK) {
				if (wifiBufferIdx != 0) {
					CompressBuffer();
					CDC_Transmit_FS((uint8_t*) &wifiBuffer, wifiBufferIdx * sizeof(float));
					wifiBufferIdx = 0;
				}
				osSemaphoreRelease(wifiBufferSemaphoreHandle);
			}

			osDelayUntil(lastTick + ticksDelay);
		}
	} else if (COMM_MODE == COMM_WIFI) {
		for (;;) {
			// WiFi: send from TCP client to TCP server.
			uint32_t lastTick = osKernelGetTickCount();

			if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, ticksDelay) == osOK) {
				if (wifiBufferIdx != 0) {
					CompressBuffer();

					// Transmit delays: can be further decreased (tested with down to 50, still worked)
					// Receive delays: ¯\_(ツ)_/¯
					// Niko: kar se tice OSa pa casov nisem nic spreminjal.

					uint8_t prejetoSporocilo[255] = {};

					HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSEND=0,255\r\n", 16, 250); // Prepare to send data. 1st param: connection ID ; 2nd param: data length.
					HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 250);
					/*CDC_Transmit_FS(prejetoSporocilo, 255);
					memset(prejetoSporocilo, 0, 255);*/

					HAL_UART_Transmit(&huart1, (uint8_t*)&wifiBuffer, wifiBufferIdx * sizeof(float), 250); // Actually send designated data.
					HAL_UART_Receive(&huart1, prejetoSporocilo, 255, 250);
					/*CDC_Transmit_FS(prejetoSporocilo, 255);
					memset(prejetoSporocilo, 0, 255);*/

					wifiBufferIdx = 0;
				}
				osSemaphoreRelease(wifiBufferSemaphoreHandle);
			}

			osDelayUntil(lastTick + ticksDelay);
		}
	}
  /* USER CODE END StartPosiljanjeWifi */
}

/* USER CODE BEGIN Header_StartTransmitPWM */
/**
* @brief Function implementing the transmitPWM thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTransmitPWM */
void StartTransmitPWM(void *argument)
{
  /* USER CODE BEGIN StartTransmitPWM */
	/* Infinite loop */
	// MOCK
	uint16_t prev_ts = 0xFFFF;
	uint8_t lastBufferIdx = 0;


	// PWM
	/*
	 uint32_t pwm_prev_packet_start = 0;
	 uint32_t start;
	 uint32_t temp;
	 uint32_t event[18];
	 */
	uint16_t PWM_total;
	uint8_t first_iter = 1;
	uint8_t ticksDelay = 18; // 20 max, some slack

	for (;;) {
		uint32_t lastTick = osKernelGetTickCount();

		if (1) {
		//if (PWM_generated_ready) {
			if (MOCK_PWM_GEN) {
				if (SEND_PWM_OUTGOING) {
					float header;
					char b[] = { 0xaa, 0xa0, 0xaa, 0xa0 };
					memcpy(&header, &b, sizeof(float));
					if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK) {
						addPWMToWifiBuffer(&PWM_generated[0], header, &lastBufferIdx);
						osSemaphoreRelease(wifiBufferSemaphoreHandle);
					}
				}

				if (SEND_THRUST_AIRSIM) {
					if (prev_ts != ts) {
						float thrust = (float) (PWM_generated[THROTTLE_CHANNEL] - PWM_LOW) / (float) (PWM_HIGH - PWM_LOW);
						CDC_Transmit_FS((uint8_t*) &thrust, sizeof(float));

						prev_ts = ts;
					}
				}

				PWM_generated_ready = 0;
			} else {
				//uint16_t PWM_demo[] = { 1421, 1421, 1421, 1421, 1421, 1421, 1421, 1421 };

				//PWM_total = PWM_demo[0];
				PWM_total = PWM_generated[0];
				delay[0] = PWM_PAUSE;
				//delay[1] = PWM_demo[0];
				delay[1] = PWM_generated[0];
				for (uint8_t i = 1; i < 8; i++) {
					delay[i * 2] = PWM_PAUSE;
					//delay[i * 2 + 1] = PWM_demo[i];
					delay[i * 2 + 1] = PWM_generated[i];
					//PWM_total += PWM_demo[i];
					PWM_total += PWM_generated[i];
				}
				delay[16] = PWM_PAUSE;
				delay[17] = PWM_PACKET_LENGTH - 9 * PWM_PAUSE - PWM_total;

				 if (first_iter) {
					 __HAL_TIM_SET_AUTORELOAD(&htim3, 1); // activate
					 first_iter = 0;
				 }
			}
		}
		osDelayUntil(lastTick + ticksDelay);
	}
  /* USER CODE END StartTransmitPWM */
}

/* USER CODE BEGIN Header_StartAltitudeMeasure */
void sync(uint16_t interval){
	// align
	while(1) {
		uint8_t a = 0xCC;
		HAL_UART_Receive(&huart2, &a, sizeof(uint8_t), interval * 2);
		if (a == 0xAA) {
			HAL_UART_Receive(&huart2, &a, sizeof(uint8_t), interval * 2);
			if (a == 0xAA){
				HAL_UART_Receive(&huart2, &a, sizeof(uint8_t), interval * 2);
				HAL_UART_Receive(&huart2, &a, sizeof(uint8_t), interval * 2);
				break;
			} else {
				HAL_UART_Receive(&huart2, &a, sizeof(uint8_t), interval * 2);
				break;
			}
		}
	}
}
/**
* @brief Function implementing the altitudeMeasure thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAltitudeMeasure */
void StartAltitudeMeasure(void *argument)
{
  /* USER CODE BEGIN StartAltitudeMeasure */
  /* Infinite loop */
	uint8_t lastBufferIdx = 0;
	float heightsMock[] = { 2.0f, 1.9997f, 1.9875f, 1.9633f, 1.9271f, 1.8788f, 1.8185f, 1.7462f, 1.6618f, 1.5655f, 1.4571f, 1.3367f };
	uint16_t interval = (int)(1000.0f * HEIGHT_SAMPLING_INTE);
	/*
	uint16_t altitude;
	uint16_t status;
	uint16_t packet[2];
	*/
	uint8_t altitude;

	// align
	//sync(interval);

  for(;;)
  {
    if (MOCK_ALTITUDE) {
    	// get height from usb cdc receive interrupt
    	/*
    	 * Height from simulation: tested, but keeping here
    	 *
    	 *
    	while(!height_ready) {
    		osDelay(2);
    	}

    	autolander_newMeasurement(height_from_simul);

    	height_ready = 0;
    	*/

    	uint32_t start_tick = osKernelGetTickCount();

    	if (SEND_ALTITUDE) {
			float data[2];
			char b[] = { 0xaa, 0xa1, 0xaa, 0xa1 };
			memcpy(&data[0], &b, sizeof(float));
			data[1] = heightsMock[heightSample_idx % 12];
			if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK) {
				addToWifiBuffer(&data[0], 2, &lastBufferIdx);
				osSemaphoreRelease(wifiBufferSemaphoreHandle);
			}
		}

    	autolander_newMeasurement(heightsMock[heightSample_idx % 12]); // mod, only need 12 samples

    	osDelayUntil(start_tick + (int)(1000.0f * HEIGHT_SAMPLING_INTE));
    } else {
    	// todo: ToF from arduino
    	/*
    	HAL_UART_Receive(&huart2, &packet[0], 2 * sizeof(uint16_t), interval * 2);
    	altitude = packet[1];
    	status = packet[0];

    	while (altitude > 2000) {
    		sync(interval);
        	HAL_UART_Receive(&huart2, &packet[0], 2 * sizeof(uint16_t), interval * 2);
        	altitude = packet[1];
        	status = packet[0];
    	}
    	*/

    	HAL_UART_Receive(&huart2, &altitude, sizeof(uint8_t), interval);

    	float heightFromToF = ((float)altitude) / 100.0f;

    	autolander_newMeasurement(heightFromToF);

    	if (SEND_ALTITUDE){
			float data[2];
			char b[] = { 0xaa, 0xa1, 0xaa, 0xa1 };
			memcpy(&data[0], &b, sizeof(float));
			data[1] = heightFromToF;
			if (osSemaphoreAcquire(wifiBufferSemaphoreHandle, 5) == osOK) {
				addToWifiBuffer(&data[0], 2, &lastBufferIdx);
				osSemaphoreRelease(wifiBufferSemaphoreHandle);
			}
		}
    }

  }
  /* USER CODE END StartAltitudeMeasure */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  if (htim->Instance == TIM3) {
	__HAL_TIM_SET_AUTORELOAD(&htim3, delay[delay_idx]); // set next delay (interleaved PWM_PAUSE and PWM_Pulse[i])
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, delay_idx % 2);
	delay_idx++; delay_idx %= 18;
  }
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
