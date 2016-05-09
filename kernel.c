int main() {
	interrupt(0x10, 0x0E00 + 'w', 0, 0, 0);
  while (1) continue;
}

int handleInterrupt21() {
  /* Placeholder */
}

int handleTimerInterrupt() {
  /* Placeholder */
}