// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Reusable composable from the core library.


package com.umicom.core
import androidx.compose.material3.*
import androidx.compose.runtime.Composable

@Composable
fun CoreHello(text:String){
  Text("Hello " + text)
}
