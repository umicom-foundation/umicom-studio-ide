// Create a small Express HTTP server with one JSON endpoint.
import express from 'express'
const app = express()
const port = 3000

// Define a simple route that returns JSON.
app.get('/api/hello', (req, res) => {
  res.json({ ok: true, message: 'Hello from Umicom Express API!' })
})

// Start the HTTP server.
app.listen(port, () => {
  console.log(`API listening on http://localhost:${port}`)
})
