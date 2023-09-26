# Door Locker Security System

This is the final project that implements a Door Locker Security System using two ATmega32 Microcontrollers with a frequency of 8MHz. The system is designed based on a layered architecture model, with two distinct units:

1. **HMI_ECU (Human Machine Interface):** Responsible for interacting with the user through a 2x16 LCD and a 4x4 keypad. It handles user inputs and displays messages on the LCD.

2. **Control_ECU:** Manages all system processing and decisions, including password verification, door control, and alarm activation. It interfaces with EEPROM, a Buzzer, and a DC-Motor.

**System Sequence:**

**Step 1 – Create a System Password**
- The LCD displays "Please Enter Password."
- The user enters a 5-digit password, with each digit represented by an asterisk (*).
- After entering the password, the system requests re-entry for confirmation.
- If the two entered passwords match, the system stores the password in EEPROM and proceeds to Step 2. If not, the user is prompted to repeat Step 1.

**Step 2 - Main Options**
- The LCD displays the main system options.

**Step 3 - Open Door +**
- The LCD prompts the user to enter the password.
- Upon entering the password, the HMI_ECU sends it to the Control_ECU, which compares it with the stored password in EEPROM.
- If the passwords match:
  - The DC-Motor rotates for 15 seconds clockwise, displaying "Door is Unlocking" on the screen.
  - The motor holds for 3 seconds.
  - The DC-Motor rotates for 15 seconds counterclockwise, displaying "Door is Locking" on the screen.

**Step 4 - Change Password -**
- The LCD prompts the user to enter the password.
- Upon entering the password, the HMI_ECU sends it to the Control_ECU, which compares it with the stored password in EEPROM.
- If the passwords match, the system proceeds to Step 1 to change the password.

**Step 5**
- If the passwords are unmatched in Step 3 (+ : Open Door) or Step 4 (- : Change Password):
  - The user is asked one more time for the password.
  - If the passwords match, the system either opens the door or changes the password in Steps 3 and 4.
  - If the passwords are still unmatched, the following actions occur:
    - The Buzzer is activated for 1 minute.
    - An error message is displayed on the LCD for 1 minute.
    - The system is locked, and no keypad inputs are accepted during this time.
    - The system returns to Step 2, displaying the main options again.

**Driver Requirements:**
- GPIO
- Keypad
- LCD (2x16)
- Timer
- UART
- I2C
- EEPROM
- Buzzer
- DC-Motor

**Microcontroller:**
- ATmega32

This project provides a comprehensive Door Locker Security System with password-based access control, alarm functionality, and an interactive user interface.

**schematic:**

![image](https://github.com/ElhassanAbdelmeged/Door-Locker-Security-System/assets/88130561/6154c13b-f005-47da-a131-8683a6841bab)
