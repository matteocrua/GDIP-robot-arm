import serial
import time

from groq_ai import identify_key, interpret_yes_no

PORT = "COM7"        # change if the port number changes
BAUD = 115200
TIMEOUT_S = 30       # max seconds to wait for the arm to finish

arduino = serial.Serial(PORT, BAUD, timeout=2)
time.sleep(2)        # Uno resets when the port opens
arduino.reset_input_buffer()


def collect_key(key_id):
    """Send the command and wait for DONE / ERROR. Returns True on success."""
    arduino.reset_input_buffer()
    arduino.write(f"KEY:{key_id}\n".encode())
    print(f"\nCollecting key {key_id}...")

    start = time.time()
    while time.time() - start < TIMEOUT_S:
        response = arduino.readline().decode(errors="ignore").strip()

        if response:
            print("Arduino:", response)   # debug line, delete once it works

        if response == "DONE":
            print("Key collected successfully!")
            return True

        if response.startswith("ERROR"):
            print("The arm could not complete the move.")
            return False

    print("Timed out waiting for the Arduino.")
    return False


def ask_yes_no(question):
    """Keeps asking until the answer is clearly yes or no."""
    while True:
        answer = input(question)
        result = interpret_yes_no(answer)
        if result == "yes":
            return True
        if result == "no":
            return False
        print("Sorry, I didn't catch that. Please say yes or no.")

while True:
    print("\nWhich key would you like?")
    print("You can choose key 1, 2 or 3.")

    user_input = input("\nYour response: ")
    key_id = identify_key(user_input)

    if key_id == 0:
        print("\nSorry, I didn't understand.")
        print("Please choose key 1, 2 or 3.")
        continue

    print(f"\nYou selected key {key_id}.")
    if ask_yes_no("Would you like me to collect it? "):
        collect_key(key_id)
    else:
        print("\nCollection cancelled.")

    if not ask_yes_no("\nWould you like another key? "):
        print("Goodbye!")
        break

arduino.close()