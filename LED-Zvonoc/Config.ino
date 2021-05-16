bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");// Открываем файл для чтения
  if (!configFile) {  // если файл не найден 
    Serial.println("Failed to open config file");
    saveConfig(); //  Создаем файл запиав в него аные по умолчанию
    configFile.close();
    return false;
  }
  size_t size = configFile.size(); // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  if (size > 1024) {
    Serial.println("Config file size is too large");
    configFile.close();
    return false;
  }
  jsonConfig = configFile.readString();// загружаем файл конфигурации в глобальную переменную
  configFile.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);  // Теперь можно получить значения из root  
    _ssidAP = root["ssidAPName"].as<String>(); // Так получаем строку
    _passwordAP = root["ssidAPPassword"].as<String>();
    timezone = root["timezone"];               // Так получаем число
    SSDP_Name = root["SSDPName"].as<String>();
    _ssid = root["ssidName"].as<String>();
    _password = root["ssidPassword"].as<String>();
    text=root["text"].as<String>();
    color=root["color"];
    return true;
}
bool saveConfig() {// Запись данных в файл config.json 
  DynamicJsonBuffer jsonBuffer;//  вызовите парсер JSON через экземпляр jsonBuffer  
  JsonObject& json = jsonBuffer.parseObject(jsonConfig); // Заполняем поля json  
  json["SSDPName"] = SSDP_Name;
  json["ssidAPName"] = _ssidAP;
  json["ssidAPPassword"] = _passwordAP;
  json["ssidName"] = _ssid;
  json["ssidPassword"] = _password;
  json["timezone"] = timezone;
  json["text"]=text;
  json["color"]=color;
  json.printTo(jsonConfig); // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
  File configFile = SPIFFS.open("/config.json", "w");// Открываем файл для записи
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    configFile.close();
    return false;
  }
  json.printTo(configFile); // Записываем строку json в файл 
  configFile.close();
  return true;
  }
String readFile(String fileName, size_t len ) {
  File configFile = SPIFFS.open("/" + fileName, "r");
  if (!configFile) {
    return "Failed";
  }
  size_t size = configFile.size();
  if (size > len) {
    configFile.close();
    return "Large";
  }
  String temp = configFile.readString();
  configFile.close();
  return temp;
}
