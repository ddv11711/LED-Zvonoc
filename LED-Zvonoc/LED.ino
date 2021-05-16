void display_updater(){
  display.display(display_draw_time);
}
void display_update_enable(bool is_enable){
  if (is_enable) display_ticker.attach(0.004, display_updater);
  else display_ticker.detach();
}
void scroll_text(uint8_t ypos, String text, uint16_t color){
    uint16_t text_length = text.length();
    display.setTextColor(color);//цвет
    if(xpos<=-(matrix_width+text_length*5))xpos=matrix_width;
      display.clearDisplay();//очистка экрана
      display.setCursor(xpos,ypos);///хх yy
      display.println(utf8rus(text));//вывод текста
      xpos--;
}
String utf8rus(String source){
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };
  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xBF){
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}
