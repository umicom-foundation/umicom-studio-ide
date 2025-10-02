// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
plugins { kotlin("jvm") version "1.9.24" }
repositories { mavenCentral() }
dependencies { testImplementation(kotlin("test")) }
tasks.register("run", JavaExec::class) {
  classpath = files("app.jar")
  mainClass.set("MainKt")
}
