void HTTP_init(void){
     HTTP.on("/restart",handle_Restart);//перезагрузка
     HTTP.on("/configs.json", handle_ConfigJSON); // формирование configs.json страницы для передачи данных в web интерфейс
     HTTP.on("/ssdp",handle_set_ssdp);
     HTTP.on("/ssid",handle_set_ssid);
     HTTP.on("/ssidap",handle_set_ssidap);
     HTTP.on("/text",handle_set_text);
     HTTP.on("/ip", handle_set_ip);
     HTTP.on ("/", [](){
             if(HTTP_ssid==""&&HTTP_password=="")HTTP.send(200, "text/html", readFile("index.html", 32000));
             if(HTTP_ssid!=""&&HTTP_password!=""){
             if (!HTTP.authenticate(HTTP_ssid.c_str(),HTTP_password.c_str()))
                return HTTP.requestAuthentication(DIGEST_AUTH, "realm", "Please Log In");
             HTTP.send(200, "text/html", readFile("index.html", 32000));}
             if(HTTP_ssid!=""&&HTTP_password==""){
             if (!HTTP.authenticate(HTTP_ssid.c_str(),""))
                return HTTP.requestAuthentication(DIGEST_AUTH, "realm", "Please Log In");
             HTTP.send(200, "text/html", readFile("index.html", 32000));}
     });
     HTTP.begin();//Запускаем HTTP-server
  }
void handle_Restart() {
  String restart = HTTP.arg("device");          // Получаем значение device из запроса
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  }
  else HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
}
void handle_set_ssdp(){
     SSDP_Name=HTTP.arg("ssdp");
     saveConfig();
     HTTP.send(200, "text/plain","OK");
}
void handle_set_ssid(){
     _ssid=HTTP.arg("ssid");
     _password=HTTP.arg("password");
     saveConfig();
     HTTP.send(200, "text/plain","OK");
}
void handle_set_ssidap(){
     _ssidAP=HTTP.arg("ssidAP");
     _passwordAP=HTTP.arg("passwordAP");
     saveConfig();
     HTTP.send(200, "text/plain","OK");
}
void handle_set_text(){
     text=HTTP.arg("text");
     saveConfig();
     HTTP.send(200, "text/plain","OK");
}
void handle_set_ip(){
  if(HTTP.arg("device")=="ok"){
     HTTP.send (200,"text/plain", "OK"); //ответ сервера
  WiFi.disconnect();
  Serial.println("disconnect");
  WiFi.mode (WIFI_STA);//Переходим в режим для подключения к wifi
     byte tries = 11;//Количество попыток подключения 
     WiFi.begin (_ssid.c_str(), _password.c_str());//подключаемся к wifi
     while (--tries && WiFi.status() != WL_CONNECTED){
           Serial.print(".");
           delay(1000);
     }
     if (WiFi.status()== WL_NO_SSID_AVAIL)ip="Сеть не найдена";
     if (WiFi.status()==6){ip="Неправильный пароль";}
     if (WiFi.status()!= WL_CONNECTED){
        //Если не удалось подключиться подымаем свою точку доступа
        Serial.println("fail");
        Serial.println("WiFi up AP");
        Serial.println("192.168.4.1");
        StartAPMode();
     }
     else {
          Serial.println("");
          Serial.println("connect");
          ip=WiFi.localIP().toString();
          Serial.println(WiFi.localIP());
          Serial.println(ip);
          saveConfig();
          StartAPMode();
     }
  }
   else HTTP.send(200,"text/plain","No IP");
}
void handle_ConfigJSON() {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  //{"SSDP":"SSDP-test","ssid":"home","password":"i12345678","ssidAP":"WiFi","passwordAP":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  json["SSDP"] = SSDP_Name;
  json["ssidAP"] = _ssidAP;
  json["passwordAP"] = _passwordAP;
  json["ssid"] = _ssid;
  json["password"] = _password;
  json["timezone"] = timezone;
  json["text"]=text;
  json["ip"] =ip;
  json["time"] = GetTime();
  json["date"] = GetDate();
  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}
