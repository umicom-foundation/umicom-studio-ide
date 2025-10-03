// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: App module with product flavors (free/pro) and Hilt DI.


plugins {
  id("com.android.application") version "8.5.0"
  id("org.jetbrains.kotlin.android") version "1.9.24"
  id("com.google.dagger.hilt.android") version "2.51"
  kotlin("kapt") version "1.9.24"
}
android {
  namespace = "com.umicom.enterprise"
  compileSdk = 34
  defaultConfig {
    applicationId = "com.umicom.enterprise"
    minSdk = 24
    targetSdk = 34
    versionCode = 1
    versionName = "1.0"
    testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
  }
  flavorDimensions += "tier"
  productFlavors {
    create("free") { applicationIdSuffix = ".free"; versionNameSuffix = "-free" }
    create("pro")  { applicationIdSuffix = ".pro";  versionNameSuffix = "-pro"  }
  }
  buildFeatures { compose = true }
  composeOptions { kotlinCompilerExtensionVersion = "1.5.14" }
  compileOptions { sourceCompatibility = JavaVersion.VERSION_17; targetCompatibility = JavaVersion.VERSION_17 }
  kotlinOptions { jvmTarget = "17" }
}
dependencies {
  implementation(platform("androidx.compose:compose-bom:2024.06.00"))
  implementation("androidx.compose.ui:ui")
  implementation("androidx.compose.material3:material3")
  implementation("androidx.activity:activity-compose:1.9.2")
  implementation(project(":lib:core"))
  implementation("com.google.dagger:hilt-android:2.51")
  kapt("com.google.dagger:hilt-android-compiler:2.51")
  testImplementation("junit:junit:4.13.2")
  androidTestImplementation("androidx.test.ext:junit:1.2.1")
  androidTestImplementation("androidx.test.espresso:espresso-core:3.6.1")
}
