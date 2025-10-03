// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
plugins {
    kotlin("jvm") version "1.9.24"
    application
}
repositories { mavenCentral() }
dependencies {
    testImplementation(kotlin("test"))
}
application {
    mainClass.set("MainKt")
}
