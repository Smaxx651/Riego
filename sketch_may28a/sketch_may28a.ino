

void setup() {
  Serial.begin(9600);

}
void loop() {
  int val;
  val = analogRead(0); 
  Serial.println(val); 
  delay(100);

  
}