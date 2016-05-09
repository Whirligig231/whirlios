int main() {
  interrupt(0x21, 0x4953, "I am a shell.", 0, 0);
  while (1) continue;
}