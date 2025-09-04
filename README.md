# LSB Steganography in C

This project demonstrates **image steganography** using the **Least Significant Bit (LSB)** technique.
It allows you to **hide secret text files inside BMP images** and later **extract them back** without noticeable changes to the image.

---

## 📂 Project Structure

```
.
├── main.c              # Entry point of the program
├── encode.c            # Implementation of encoding (hiding secret data)
├── encode.h            # Header file for encode functions
├── decode.c            # Implementation of decoding (extracting hidden data)
├── decode.h            # Header file for decode functions
├── types.h             # Common data structures
├── common.h            # Shared macros / constants
├── test_encode.c       # Test cases for encoding functions
├── beautiful.bmp       # Sample cover image
├── secret.txt          # Example secret text file
├── steged_beautiful.bmp# Example stego image with hidden message
├── my_msg.txt          # Example message for testing
├── README.md           # Documentation (this file)
└── .vscode/            # VS Code project settings
```

---

## ⚙️ How It Works

* **Encoding**:

  * Takes a **BMP image** and a **text file** as input.
  * Hides the text inside the image by modifying the **least significant bit (LSB)** of pixel data.
  * Produces a new **stego image** that looks almost identical to the original.

* **Decoding**:

  * Takes a **stego image** as input.
  * Extracts the hidden message from the LSBs and writes it to an output file.

---

## 🛠️ Build Instructions

### Compile

Use GCC to build:

```bash
gcc -Wall -Wextra main.c encode.c decode.c -o lsb_steg
```

### Run

#### Encode (Hide Secret)

```bash
./lsb_steg -e <source_image.bmp> <secret_file.txt> <stego_image.bmp>
```

Example:

```bash
./lsb_steg -e beautiful.bmp secret.txt stego_out.bmp
```

#### Decode (Extract Secret)

```bash
./lsb_steg -d <stego_image.bmp> <output_secret.txt>
```

Example:

```bash
./lsb_steg -d stego_out.bmp recovered_secret.txt
```

---

## 🧪 Example

1. **Input**:

   * Image: `beautiful.bmp` (cover image)
   * Secret: `secret.txt` (contains: *"My name is REHAN Mokashi from ADCET, Ashta"*)

2. **Encoding**:

   ```bash
   ./lsb_steg -e beautiful.bmp secret.txt stego_out.bmp
   ```

   → Output: `stego_out.bmp`

3. **Decoding**:

   ```bash
   ./lsb_steg -d stego_out.bmp recovered_secret.txt
   ```

   → Output file contains:

   ```
   My name is REHAN Mokashi from ADCET, Ashta
   ```

---

## 🧩 Makefile (optional)

If you want `make` convenience, add this `Makefile` to the repo root:

```makefile
CC=gcc
CFLAGS=-Wall -Wextra
SRC=main.c encode.c decode.c
OUT=lsb_steg

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT) *.o

.PHONY: all clean
```

Use `make` to build and `make clean` to remove the binary.

---

## 📌 Features

* Supports hiding text inside **24-bit BMP images**.
* Recovers text with **lossless accuracy**.
* Modular C code (`encode.c`, `decode.c`) with headers for easy understanding.
* Example inputs provided.

---

## 🚀 Future Improvements

* Support for more image formats (PNG, JPEG).
* Encrypt the secret before embedding.
* GUI frontend for easy use.
* Better error handling and validations.

---

## ⚠️ Notes

* Works only with **uncompressed 24-bit BMP images**.
* The **size of the secret text** must be smaller than the **capacity of the image**.
* Avoid committing large binaries to GitHub; build from source instead.

---

## 📜 License

This project is for **educational purposes**.
You may use or modify it under the terms of the [MIT License](https://opensource.org/licenses/MIT).
