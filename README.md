# ChocoDefender

## Introduction

ChocoDefender is an ressource-efficient protection against Chocoblasting. It interceps keyboards events to detect the keyword of your choice and lock the screen.

## Install

```bash
git clone https://github.com/killianmarty/ChocoDefender
cd ChocoDefender
make
```

## Usage

Simply run the compiled binary with sudo rights.

```bash
sudo ./ChocoDefender
```

You can also specify the keyword you want to detect (by default it is "choco"):

```bash
sudo ./ChocoDefender <your_keyword>
```

## Disclaimer

This is not an absolute protection, it can be bypassed many ways, always lock you screen when you leave (unless you are generous with your colleagues).

## Credits

This project was made by Killian Marty.