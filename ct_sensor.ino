/*
 * Minimize the amount of code to optimize the amount of samples collected
*/

// hold sampled ADC values of the voltages from the voltage and current sensors
int analogV = 0;
int analogI = 0;

// Hold the converted values from the sampled ADC values to a voltage value
float Ivoltage;
float Vvoltage;

// Arrays of the sampled ADC values
float iArr[100];
float vArr[100];

// Array of the time it takes to sample
long sampleTime;

// Hold the RMS value of the current, voltage and power from the array of sampled values
float CurrentRMS;
float VoltageRMS;
float PowerRMS;
// the energy consumed by the device over the range of time sampled
float energy;

// Keeps track of the number of samples taken
int count;

// Keeps track of the time between samples; confines the sampling time
long oldTime = 0;
long newTime = 0;

// Counts down time so as to display the values
long oldDisplayTime = 0;
long newDisplayTime = 0;

// Boolean to keep track of when all the samples are collected before they are displayed
bool terminate;


void setup() {
    Serial.begin(9600);
    CurrentRMS = 0.0;
    VoltageRMS = 0.0;
    PowerRMS = 0.0;
    energy = 0.0;
    count = 0;

    terminate = false;

}

void loop() {
    // ******************Sampling block***********************
    // 2ms non-blocking delay using millis()
    newTime = millis();
    if(newTime >= (oldTime+2)){
        //ADC value for voltage transformer
        analogV = analogRead(A1);
        //ADC value for current sensor
        analogI = analogRead(A2);

        vArr[count] = analogV;
        iArr[count] = analogI;

        //To keep track of time
        oldTime = newTime;
    }// End of sampling
  
    // ********************Count block**************************
    // Keep track of the count
    count++;
    if(count >= 100){
        terminate = true;
    }// End of count

    // *****************Calculation block***********************
    // Calculate the RMS values of the current, voltage
    // and power together with the energy values
    if(terminate){

        for(int i=0; i<count; i++){
            //Convert the current sensor ADC value to volts within the range of 0v - 5v
            //The input is offset by DC 2.50v and thus it has to be accounted for.
            Ivoltage = (iArr[i]*(5.0/1024))-2.50;
            float iRMS = ((Ivoltage/30.0)*2000.0)/4.0;
            CurrentRMS += iRMS*iRMS;

            //Convert the voltage transformer ADC value to volts within the range of 0v - 5v
            //The input is offset by DC 2.50v and thus it has to be accounted for.
            Vvoltage = (vArr[i]*(5.0/1024))-2.50;
            float vRMS = (110/10)*analogV*(230/11);
            VoltageRMS += vRMS*vRMS;
        }
        
        CurrentRMS = sqrt(CurrentRMS/count);
        /*if(CurrentRMS <0.2){
        CurrentRMS = 0.0;
        }*/
        VoltageRMS = sqrt(VoltageRMS/count);
        // Power in KW used by the device
        PowerRMS = (VoltageRMS * CurrentRMS * 0.8)/1000;
        // Energy, in KWh, consumed within the second
        energy += PowerRMS * (1/3600);

    //    count = 0;
    }// End of calculation

    //***********************Calculation block*********************
    // Display the calculated values
    newDisplayTime = millis();
    if((newDisplayTime-oldDisplayTime) >= 1000){
        oldDisplayTime = newDisplayTime;
        Serial.print("Vrms= ");
        Serial.print(VoltageRMS);
        Serial.println("V");
        VoltageRMS = 0.0;

        Serial.print("Irms= ");
        Serial.print(CurrentRMS);
        Serial.println("A");
        CurrentRMS = 0.0;

        Serial.print("Power= ");
        Serial.print((PowerRMS), 5);
        Serial.println("W");
        PowerRMS = 0.0;
        
        Serial.print("Energy= ");
        Serial.print(energy, 9);
        Serial.println("KWh");
        Serial.println("");

        terminate = false;
        count = 0;
        
    }// end of display

}//end of loop()

