# 🖼️ LSB Steganography Tool in C

This project implements **Least Significant Bit (LSB) Steganography** in C to hide and retrieve secret messages within BMP image files. It supports both **encoding** (hiding text data) and **decoding** (extracting it back), with detailed logging and error handling.

---

## ✅ Features

- Embed secret `.txt` files inside `.bmp` images
- Decode and extract hidden messages
- Preserves original BMP headers
- Validates image capacity and file existence
- Auto-generates output file if not provided
- Informative debug logs for each step
- Designed for Linux (GCC)

---

## 📂 Project Structure

| File          | Purpose                               |
|---------------|----------------------------------------|
| `main.c`      | Entry point, handles CLI args          |
| `encode.c`    | LSB encoding implementation            |
| `decode.c`    | LSB decoding implementation            |
| `encode.h`    | Function declarations + structures     |
| `decode.h`    | Function declarations + structures     |
| `README.md`   | Project overview and usage guide       |

---

## ⚙️ Compilation

Use the following command to build the project:

```bash
gcc -DDEBUG main.c encode.c decode.c -o lsb_steg -Wall -Wextra

---

## Encoding: Hide a secret text file in a BMP image
./lsb_steg -e beautiful.bmp secret.txt steged_beautiful.bmp

---

## Examples:
bash
./lsb_steg -e beautiful.bmp secret.txt steged_beautiful.bmp
./lsb_steg -e beautiful.bmp secret.txt


## Decoding: Extract the hidden secret file from a stego image
./lsb_steg -d <stego_image.bmp> <output_secret.txt>
./lsb_steg -d steged_beautiful.bmp secret_msg.txt


Encode (with default output file)
INFO: Output File not mentioned. Creating steged_img.bmp as default
INFO: Opening required files
INFO: Opened beautiful.bmp
INFO: Opened secret.txt
INFO: Opened steged_img.bmp
INFO: Done
INFO: ## Encoding Procedure Started ##
INFO: Checking for secret.txt size
INFO: Done. Not Empty
INFO: Checking for beautiful.bmp capacity to handle secret.txt
INFO: Done. Found OK
INFO: Copying Image Header
INFO: Done
INFO: Encoding Magic String Signature
INFO: Done
INFO: Encoding secret.txt File Extenstion
INFO: Done
INFO: Encoding secret.txt File Size
INFO: Done
INFO: Encoding secret.txt File Data
INFO: Done
INFO: Copying Left Over Data
INFO: Done
INFO: ## Encoding Done Successfully ##


Decode:
INFO: Selected decoding
INFO: ## Decoding Procedure Started ##
INFO: Opening required files
INFO: Opened steged_beautiful.bmp
INFO: Decoding Magic String Signature
INFO: Done
INFO: Decoding Output File Extension
DEBUG: Decoded extension length: 4
INFO: Done
INFO: Opened secret_msg.txt
INFO: Done. Opened all required files
INFO: Decoding secret_msg.txt File Size
INFO: Done
INFO: Decoding secret_msg.txt File Data
INFO: Done
INFO: ## Decoding Done Successfully ##
