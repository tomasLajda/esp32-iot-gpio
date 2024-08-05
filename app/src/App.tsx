import { useEffect, useState } from 'react';
import './App.css';
import reactLogo from './assets/react.svg';
import { Pin, readPins, setPin } from './firebase'; // Import the functions from your Firebase module
import viteLogo from '/vite.svg';

const App = () => {
  const [count, setCount] = useState(0);
  const [pins, setPins] = useState<Pin[]>([]);

  useEffect(() => {
    const pinIds = [16, 17, 18, 19];
    readPins(pinIds, (updatedPins: Pin[]) => {
      setPins(updatedPins);
    });
  }, []);

  return (
    <>
      <div>
        <a href='https://vitejs.dev' target='_blank'>
          <img src={viteLogo} className='logo' alt='Vite logo' />
        </a>
        <a href='https://react.dev' target='_blank'>
          <img src={reactLogo} className='logo react' alt='React logo' />
        </a>
      </div>
      <h1>Vite + React</h1>
      <div className='card'>
        <button onClick={() => setCount((count) => count + 1)}>
          count is {count}
        </button>
        <p>
          Edit <code>src/App.tsx</code> and save to test HMR
        </p>
      </div>
      <div className='pins'>
        {pins.map((pin) => (
          <div key={pin.id}>
            <p>
              Pin {pin.id}: {pin.value}
            </p>
            <button onClick={() => setPin({ ...pin, value: pin.value + 1 })}>
              Increment Pin {pin.id}
            </button>
          </div>
        ))}
      </div>
      <p className='read-the-docs'>
        Click on the Vite and React logos to learn more
      </p>
    </>
  );
};

export default App;
