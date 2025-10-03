// Replace the placeholder message text once the page loads.
const msg = document.getElementById('msg');            // Locate the paragraph by its id
msg.textContent = 'Hello from Vanilla JS in Umicom Studio IDE!'; // Update its content

// Wire up a click handler to show basic interactivity.
const btn = document.getElementById('btn');            // Find the button element
btn.addEventListener('click', () => {                   // When the button is clicked...
  alert('You clicked the demo button!');                // ...show a small alert popup
});
