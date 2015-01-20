/***************************************************************************************************************
* Razor AHRS Firmware v1.4.0
* 9 Degree of Measurement Attitude and Heading Reference System
* for Sparkfun "9DOF Razor IMU" (SEN-10125 and SEN-10736)
* and "9DOF Sensor Stick" (SEN-10183, 10321 and SEN-10724)
*
* Released under GNU GPL (General Public License) v3.0
* Copyright (C) 2011 Quality & Usability Lab, Deutsche Telekom Laboratories, TU Berlin
*
* Infos, updates, bug reports and feedback:
*     http://dev.qu.tu-berlin.de/projects/sf-razor-9dof-ahrs
*/
// How to calibrate? Read the tutorial at http://dev.qu.tu-berlin.de/projects/sf-razor-9dof-ahrs

/*
  "9DOF Razor IMU" hardware versions: SEN-10125 and SEN-10736

  ATMega328@3.3V, 8MHz

  ADXL345  : Accelerometer
  HMC5843  : Magneto on SEN-10125
  HMC5883L : Magneto on SEN-10736
  ITG-3200 : Gyro

  Arduino IDE : Select board "Arduino Pro or Pro Mini (3.3v, 8Mhz) w/ATmega328"
*/

/*
  "9DOF Sensor Stick" hardware versions: SEN-10183, SEN-10321 and SEN-10724

  ADXL345  : Accelerometer
  HMC5843  : Magneto on SEN-10183 and SEN-10321
  HMC5883L : Magneto on SEN-10724
  ITG-3200 : Gyro
*/

/*
  Axis definition (differs from definition printed on the board!):
    X axis pointing forward (towards the short edge with the connector holes)
    Y axis pointing to the right
    and Z axis pointing down.
    
  Positive yaw   : clockwise
  Positive roll  : right wing down
  Positive pitch : nose up
  
  Transformation order: first yaw then pitch then roll.
*/

#include "lpc_types.h"
#include "FreeRTOS.h"
#include "AHRS.h"
#include "semphr.h"
#include <Math.h>
#include "Vector.h"
#include "DCM.h"
#include "Gyro.h"
#include "Accel.h"
#include "Magneto.h"
#include "Output.h"
#include "XBee.h"
#include "utils.h"
#include "term_io.h"
#include "USB.h"

float G_Dt; // Integration time for DCM algorithm

// More output-state variables
int curr_calibration_sensor = 0;
uint8_t reset_calibration_session_flag = TRUE;
uint8_t mode = 0;

uint8_t output_stream_on = FALSE;

static xSemaphoreHandle AHRS_lock;

void updateSensors() {
	Gyro_Update();
	Accel_Update();
	Magneto_Update();
}

// Read every sensor and record a time stamp
// Init DCM with unfiltered orientation
// TODO re-init global vars?
void reset_sensor_fusion() {
  float temp1[3];
  float temp2[3];
  float xAxis[] = {1.0f, 0.0f, 0.0f};

  updateSensors();
  
  // GET PITCH
  // Using y-z-plane-component/x-component of gravity vector
  pitch = -atan2(Accel_Get(0), sqrt(Accel_Get(1) * Accel_Get(1) + Accel_Get(2) * Accel_Get(2)));
	
  // GET ROLL
  // Compensate pitch of gravity vector 
  temp2[0] = Accel_Get(0);
  temp2[1] = Accel_Get(1);
  temp2[2] = Accel_Get(2);
  Vector_Cross_Product(temp1, temp2, xAxis);
  Vector_Cross_Product(temp2, xAxis, temp1);
  // Normally using x-z-plane-component/y-component of compensated gravity vector
  // roll = atan2(temp2[1], sqrt(temp2[0] * temp2[0] + temp2[2] * temp2[2]));
  // Since we compensated for pitch, x-z-plane-component equals z-component:
  roll = atan2(temp2[1], temp2[2]);
  
  // GET YAW
  yaw = Magneto_GetHeading();
  
  // Init rotation matrix
  init_rotation_matrix(DCM_Matrix, yaw, pitch, roll);
}

// Reset calibration session if reset_calibration_session_flag is set
void check_reset_calibration_session()
{
  // Raw sensor values have to be read already, but no error compensation applied

  // Reset this calibration session?
  if (!reset_calibration_session_flag) return;
  
  // Reset acc and mag calibration variables
  for (uint8_t i = 0; i < 3; i++) {
    accel_min[i] = accel_max[i] = Accel_GetRaw(i);
    magneto_min[i] = magneto_max[i] = Magneto_GetRaw(i);
  }

  // Reset gyro calibration variables
  gyro_num_samples = 0;  // Reset gyro calibration averaging
  gyro_average[0] = gyro_average[1] = gyro_average[2] = 0.0f;
  
  reset_calibration_session_flag = FALSE;
}

double AHRS_Get(uint8_t axis){
	if(axis == ROLL){
		return roll;
	}else if(axis == PITCH){
		return pitch;
	}else if(axis == YAW){
		return yaw;
	}else{
		return 0;
	}
}

double AHRS_GetRoll(){
	return roll;
}

double AHRS_GetPitch(){
	return pitch;
}

double AHRS_GetYaw(){
	return yaw;
}

void AHRS_NextSensor(){
	if(xSemaphoreTake(AHRS_lock, portMAX_DELAY) == pdTRUE){
		if (mode == MODE_CALIBRATE_SENSORS){
			curr_calibration_sensor = (curr_calibration_sensor + 1) % 3;
			reset_calibration_session_flag = TRUE;
		}else if (mode == MODE_SENSORS_TEXT){
			curr_calibration_sensor = (curr_calibration_sensor + 1) % 2;
			if (curr_calibration_sensor == 0){
				xputs("Scaled\n");;
			}else if (curr_calibration_sensor == 1){
				xputs("Raw\n");
			}
		}

		xSemaphoreGive(AHRS_lock);
	}
}

void AHRS_ModeCalibrate(){
	if(xSemaphoreTake(AHRS_lock, portMAX_DELAY) == pdTRUE){
		mode = MODE_CALIBRATE_SENSORS;
		reset_calibration_session_flag = TRUE;

		xSemaphoreGive(AHRS_lock);
	}
}

void AHRS_ModeDebug(){
	if(xSemaphoreTake(AHRS_lock, portMAX_DELAY) == pdTRUE){
		mode = MODE_SENSORS_TEXT;

		xSemaphoreGive(AHRS_lock);
	}
}

void AHRS_ModeNormal(){
	if(xSemaphoreTake(AHRS_lock, portMAX_DELAY) == pdTRUE){
		mode = MODE_SENSORS_NORMAL;

		xSemaphoreGive(AHRS_lock);
	}
}

void AHRS_Init(){
	AHRS_lock = xSemaphoreCreateMutex();

	// Init sensors
	vTaskSuspendAll();
	vTaskDelay(50);  // Give sensors enough time to start
	Accel_Init();
	Gyro_Init();
	Magneto_Init();

	// Read sensors, init DCM algorithm
	vTaskDelay(20);  // Give sensors enough time to collect data
	reset_sensor_fusion();
	xTaskResumeAll ();
}

static portSTACK_TYPE ahrsTaskStack[256];
const xTaskParameters ahrsTaskDefinition =
{
	AHRS_Run,   							/* pvTaskCode */
	(signed char *) "AHRS",  	/* pcName */
    256,             				/* usStackDepth - defined in words, not bytes. */
    NULL,            				/* pvParameters */
    7 | portPRIVILEGE_BIT,  /* uxPriority */
    ahrsTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

// Main loop
void AHRS_Run(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();
	portTickType xLastUpdate = xLastWakeTime;
	portTickType xLastAHRSSend = xLastWakeTime;
	portTickType xLastDebug = xLastWakeTime;

	while(1){
		if(xSemaphoreTake(AHRS_lock, portMAX_DELAY) == pdTRUE){
			portTickType old = xLastUpdate;
			xLastUpdate = xTaskGetTickCount();
			if (xLastUpdate > old){
				G_Dt = (float) (xLastUpdate - old) / 1000.0f; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
			}else {
				G_Dt = 0;
			}

			updateSensors();

			if (mode == MODE_CALIBRATE_SENSORS){  // We're in calibration mode
				for (uint8_t i = 0; i < 3; i++) {
					if (Accel_GetRaw(i) < accel_min[i]){
						accel_min[i] = Accel_GetRaw(i);
					}

					if (Accel_GetRaw(i) > accel_max[i]){
						accel_max[i] = Accel_GetRaw(i);
					}

				}

				for (uint8_t i = 0; i < 3; i++) {
					if (Magneto_GetRaw(i) < magneto_min[i]){
						magneto_min[i] = Magneto_GetRaw(i);
					}
					if (Magneto_GetRaw(i) > magneto_max[i]){
						magneto_max[i] = Magneto_GetRaw(i);
					}
				}

				for (uint8_t i = 0; i < 3; i++){
					gyro_average[i] += Gyro_GetRaw(i);
				}
				gyro_num_samples++;

				if(xLastWakeTime - xLastDebug > 200){
					check_reset_calibration_session();  // Check if this session needs a reset

					output_calibration(curr_calibration_sensor);
					xLastDebug = xLastWakeTime;
				}
			}else if (mode == MODE_SENSORS_TEXT){
				if(xLastWakeTime - xLastDebug > 200){
					if (curr_calibration_sensor == 0){
						output_debug();
					}else if (curr_calibration_sensor == 1){
						output_raw();
					}

					xLastDebug = xLastWakeTime;
				}
			}else{
				// Run DCM algorithm
				Matrix_update();
				Normalize();
				Drift_correction();
				Euler_angles();
			}

			if(xLastWakeTime - xLastAHRSSend > (USB_isConnected() ? 200 : 500)){
				int16_t r = TO_DEG(roll) * 64;
				int16_t p = TO_DEG(pitch) * 64;
				int16_t y = TO_DEG(yaw) * 64;
				int16_t h = TO_DEG(Magneto_GetHeading()) * 64;
				uint8_t d[9] = {'A', r >> 8, r, p >> 8, p, y >> 8, y, h >> 8, h,};
				XBee_Send(d, 9);
				xLastAHRSSend = xLastWakeTime;
			}

			xSemaphoreGive(AHRS_lock);
		}

		vTaskDelayUntil(&xLastWakeTime, 20 / portTICK_RATE_MS);
	}
}
