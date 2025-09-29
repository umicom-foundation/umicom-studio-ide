
const log = document.getElementById('log');
const promptEl = document.getElementById('prompt');
const modelEl = document.getElementById('model');
const backendEl = document.getElementById('backend');
const sendBtn = document.getElementById('send');
const clearBtn = document.getElementById('clear');

function append(role, text) {
  const div = document.createElement('div');
  div.className = role;
  div.textContent = text;
  log.appendChild(div);
  log.scrollTop = log.scrollHeight;
}

sendBtn.onclick = async () => {
  const prompt = promptEl.value.trim();
  if (!prompt) return;
  append('you', prompt);
  promptEl.value = '';
  const model = modelEl.value.trim() || null;
  try {
    const res = await fetch('/chat', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({prompt, model})
    });
    const data = await res.json();
    if (data.ok) append('ai', data.reply);
    else append('err', data.error || 'Unknown error');
  } catch (e) {
    append('err', String(e));
  }
};

clearBtn.onclick = () => { log.innerHTML = ''; };

promptEl.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') sendBtn.click();
});
