<template>
  <main style="font-family: system-ui, sans-serif; margin: 2rem;">
    <h1 style="color:#2a5">Umicom — Vue + Vite</h1>
    <p>A minimal Vue starter using Vite.</p>
    <button @click="count++">Clicked {{ count }} times</button>
  </main>
</template>
<script setup>
import { ref } from 'vue'
const count = ref(0)
</script>
