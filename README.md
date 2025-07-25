# VRoneEduDemo

## Project Information

This project is developed for **VRone. Edu** and **VRone. 4K (Gen2) Devices** by **QWR Interactive Solutions Pvt. Ltd.**  
It also depends on **WaveVR SDK 3.2.0**.

## Required Tools & Versions

- **Android NDK:** r21b
- **JDK:** 11 (Eclipse Adoptium recommended)
- **Android SDK Platform:** android-21
- **NDK Level:** android-21
- **Minimum SDK Level:** 26
- **Target SDK Level:** 26
- **WaveVR SDK:** 3.2.0

## Installation Instructions

### 1. Install JDK 11 (Eclipse Adoptium)
Download and install from [Adoptium JDK 11](https://adoptium.net/temurin/releases/?version=11).

### 2. Install Android NDK r21b
Download NDK r21b from the [NDK Archives](https://developer.android.com/ndk/downloads/older_releases).  
Extract and set the NDK path in your environment variables.

### 3. Install Android SDK Platform 21 and SDK 28
Open a terminal and run: <sdk-manager-path>/sdkmanager "platforms;android-21" "platforms;android-28"


### 4. Install Android Command Line Tools
Download from the [Android developer site](https://developer.android.com/studio#command-tools).  
Extract to your SDK directory under `cmdline-tools/latest`.

### 5. Install WaveVR SDK 3.2.0
Download WaveVR SDK 3.2.0 from [VIVE Developer Portal](https://developer.vive.com/resources/).  
Follow the integration guide for Unreal Engine 4.25 and Android.  
Add the SDK to your project’s Plugins or ThirdParty directory as required.

### 6. Unreal Engine 4.25 Setup
- In Unreal Engine 4.25, open __Edit > Project Settings > Platforms > Android__.
- Set the paths for JDK, NDK, and SDK as per above installations.
- Ensure __SDK API Level__ and __NDK API Level__ are set to `android-21`.
- Set __Minimum SDK Version__ and __Target SDK Version__ to `26`.

## Environment Variables Example
export JAVA_HOME=/path/to/jdk-11 export ANDROID_NDK_HOME=/path/to/android-ndk-r21b export ANDROID_HOME=/path/to/android-sdk

## Notes

- Ensure all paths are correctly set in Unreal Engine and your system environment.
- For Unreal Engine Android builds, both SDK 21 and SDK 28 must be installed.
- Integrate WaveVR SDK 3.2.0 as per official documentation for VRone devices.