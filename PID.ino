#include <Wire.h>
#include <SparkFunMLX90640.h>
#define REFRESH_RATE 8
MLX90640 myMLX90640;
float mlx90640To[768];
paramsMLX90640 mlx90640;
float setpoint = 40.0; 
float maxTemp = 0.0;   
float Kp = 10.0;       
float Ki = 0.1;        
float Kd = 1.0;        
float ierr = 0.0;      
float prevError = 0.0; 
float controlOutput = 0.0; 
int heaterPin = 9; 
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
  ierr += error;                            
  float dError = error - prevError;         
  prevError = error;                        
  controlOutput = (Kp * error) + (Ki * ierr) + (Kd * dError);
  controlOutput = constrain(controlOutput, 0, 100); 
  analogWrite(heaterPin, (int)(controlOutput * 255 / 100));
  unsigned long currentTime = millis() / 1000;
  Serial.print("Tiempo");
  Serial.print(currentTime);
  Serial.print("Temperatura");
  Serial.print(maxTemp, 2);
  Serial.print("Control Output");
  Serial.println(controlOutput);
}
