import '../styles/style.css';

// TODO: make this actually call the model lol
const evalButton = document.querySelector<HTMLButtonElement>('#eval')!;
evalButton.addEventListener('click', () => console.log('eval!'));
