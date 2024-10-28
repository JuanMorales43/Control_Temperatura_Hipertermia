#include <Wire.h>
#include <SparkFunMLX90640.h>
#define REFRESH_RATE 8 
MLX90640 myMLX90640;  

float mlx90640To[768]; 
paramsMLX90640 mlx90640; 
float setpoint = 40.0;       
float maxTemp = 0.0;         
float lastTemp = 0.0;        
float lastTemp2 = 0.0;       
float controlOutput = 0.0;   
float lastOutput = 0.0;      
float Kc = 10.0;       
float tauI = 50.0;     
float tauD = 1.0;      
float KI = Kc / tauI;  
float KD = Kc * tauD;  
float KP = Kc;         
int heaterPin = 9;     
float outputMin = 0.0; 
float outputMax = 100.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (myMLX90640.begin(MLX90640_I2C_ADDR, &Wire) != 0) {
    Serial.println("Error al iniciar MLX90640. Verifique las conexiones.");
    while (1);
  }
  myMLX90640.setRefreshRate(REFRESH_RATE);
  if (myMLX90640.getDeviceParameters(&mlx90640) != 0) {
    Serial.println("Error al obtener los parámetros de la cámara.");
    while (1);
  }
  pinMode(heaterPin, OUTPUT); 
  Serial.println("MLX90640 Iniciado correctamente.");
}

void loop() {
  if (myMLX90640.getFrameData(mlx90640To, &mlx90640) != 0) {
    Serial.println("Error al obtener datos de temperatura.");
    return;
  }
  maxTemp = mlx90640To[0];
  for (int i = 1; i < 768; i++) {
    if (mlx90640To[i] > maxTemp) {
      maxTemp = mlx90640To[i];
    }
  }
  float error = setpoint - maxTemp;            
  float error_last = setpoint - lastTemp;      
  float error_last2 = setpoint - lastTemp2;    
  controlOutput = lastOutput + KP * (error - error_last) 
                  + KI * error_last 
                  + KD * (error - 2 * error_last + error_last2);

  controlOutput = constrain(controlOutput, outputMin, outputMax);
  analogWrite(heaterPin, (int)(controlOutput * 255 / 100));
  lastTemp2 = lastTemp;
  lastTemp = maxTemp;
  lastOutput = controlOutput;
  unsigned long currentTime = millis() / 1000;
  Serial.print("Tiempo");
  Serial.print(currentTime);
  Serial.print("Temperatura Máxima: ");
  Serial.print(maxTemp, 1);
  Serial.print(" °C, Control Output: ");
  Serial.println(controlOutput);
  delay(1000 / REFRESH_RATE);
}
