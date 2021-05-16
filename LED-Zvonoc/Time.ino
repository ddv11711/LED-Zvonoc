void Time_init() {
  HTTP.on("/Time", handle_Time);     // Синхронизировать время устройства по запросу вида /Time
  HTTP.on("/TimeZone", handle_time_zone);    // Установка времянной зоны по запросу вида http://192.168.0.101/TimeZone?timezone=3
  timeSynch();
}
void timersInit() {
  unsigned long uptimeMils = millis();
 // unsigned long uptimeSec = millis() / 1000;
  timer50mils=uptimeMils;
 /* timer3s = uptimeSec;  
  timer5s = uptimeSec; 
  timer10s = uptimeSec;
  timer30s = uptimeSec;
  timer1m = uptimeSec;
  timer3m = uptimeSec;
  timer24h = uptimeSec;*/
}
void timersWorks() {
  unsigned long timeMils = millis();
 // unsigned long timeSec = millis() / 1000;
  if (timeMils - timer50mils > 50) {timer50mils =  timeMils; cycle50mils = true;}
 /* if (timeSec - timer3s > 2) {timer3s =  timeSec; cycle3s = true;}
  if (timeSec - timer5s > 4) {timer5s = timeSec; cycle5s = true;}
  if (timeSec - timer10s > 9) {timer10s = timeSec; cycle10s = true;}
  if (timeSec - timer30s > 29) {timer30s = timeSec; cycle30s = true;}
  if (timeSec - timer1m >  59) {timer1m =  timeSec; cycle1m = true;}
  if (timeSec - timer3m > 179) {timer3m =  timeSec; cycle3m = true;}
  if (timeSec - timer24h > 86399) {timer24h =  timeSec; cycle24h = true;}*/}
void eraseCyclos() {
  cycle50mils=false;
 /* cycle3s = false;
  cycle5s = false;
  cycle10s = false;
  cycle30s = false;
  cycle1m = false;
  cycle3m = false;
  cycle24h = false;*/
}
void timeSynch(){
  if (WiFi.status() == WL_CONNECTED) {
    configTime(timezone * 3600, 0, "pool.ntp.org", "ru.pool.ntp.org");// Настройка соединения с NTP сервером
    int i = 0;
    Serial.println("Waiting for time");
    while (!time(nullptr) && i < 10) {
      Serial.print(".");
      i++;
      delay(1000);
    }
    Serial.println("");
    Serial.println("Time Ready!");
    Serial.println(GetTime());
    Serial.println(GetDate());
  }
}
void handle_time_zone() {               
  timezone = HTTP.arg("timezone").toInt(); // Получаем значение timezone из запроса конвертируем в int сохраняем в глобальной переменной
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
void handle_Time(){
  timeSynch();
  HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}
String GetTime() {// Получение текущего времени
 time_t now = time(nullptr); // получаем время с помощью библиотеки time.h
 String Time = ""; // Строка для результатов времени
 Time += ctime(&now); // Преобразуем время в строку формата Thu Jan 19 00:55:35 2017
 int i = Time.indexOf(":"); //Ишем позицию первого символа :
 Time = Time.substring(i - 2, i + 6); // Выделяем из строки 2 символа перед символом : и 6 символов после
 return Time; // Возврашаем полученное время
}
String GetDate() {// Получение даты
 time_t now = time(nullptr); // получаем время с помощью библиотеки time.h
 String Data = ""; // Строка для результатов времени
 Data += ctime(&now); // Преобразуем время в строку формата Thu Jan 19 00:55:35 2017
 int i = Data.lastIndexOf(" "); //Ишем позицию последнего символа пробел
 String Time = Data.substring(i - 8, i+1); // Выделяем время и пробел
 Data.replace(Time, ""); // Удаляем из строки 8 символов времени и пробел
 return Data; // Возврашаем полученную дату
}
