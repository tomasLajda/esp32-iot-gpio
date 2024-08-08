import { useEffect, useState } from 'react';
import './App.css';
import reactLogo from './assets/react.svg';
import SelectPin from './components/select-pin';
import { Pin, readPins } from './firebase'; // Import the functions from your Firebase module
import viteLogo from '/vite.svg';

interface Pins {
  inputPins: Pin[];
  outputPins: Pin[];
  analogPins: Pin[];
}

const App = () => {
  const [count, setCount] = useState(0);
  const [pins, setPins] = useState<Pins>({
    inputPins: [],
    outputPins: [],
    analogPins: [],
  });

  useEffect(() => {
    const pinIds = [16, 17, 18, 19];
    readPins(pinIds, (updatedPins: Pin[]) => {
      const sortedPins: Pins = {
        inputPins: [],
        outputPins: [],
        analogPins: [],
      };

      updatedPins.map((pin) => {
        if (pin.type == 'input') {
          sortedPins.inputPins.push(pin);
        } else if (pin.type == 'output') {
          sortedPins.outputPins.push(pin);
        } else if (pin.type == 'analog') {
          sortedPins.analogPins.push(pin);
        }
      });
      setPins(sortedPins);
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
      <p className='read-the-docs'>
        Click on the Vite and React logos to learn more
      </p>
      <SelectPin pins={pins.outputPins} type='output'></SelectPin>
      <SelectPin pins={pins.inputPins} type='input'></SelectPin>
      <SelectPin pins={pins.analogPins} type='analog'></SelectPin>
    </>
  );
};

export default App;
