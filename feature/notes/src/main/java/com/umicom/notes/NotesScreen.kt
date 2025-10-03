// Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
Purpose: Simple in-memory Notes UI; demonstrates state and list rendering.


package com.umicom.notes
import androidx.compose.runtime.*
import androidx.compose.material3.*
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun NotesScreen(){
  var input by remember { mutableStateOf("") }
  val items = remember { mutableStateListOf<String>() }
  Column(Modifier.padding(16.dp)) {
    OutlinedTextField(value=input, onValueChange={input=it}, label={ Text("Note") })
    Spacer(Modifier.height(8.dp))
    Button(onClick={ if(input.isNotBlank()){ items.add(input.trim()); input="" } }){ Text("Add") }
    Spacer(Modifier.height(16.dp))
    LazyColumn {
      itemsIndexed(items) { idx, note ->
        Text("${idx+1}. ${note}")
      }
    }
  }
}
