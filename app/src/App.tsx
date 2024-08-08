import { useEffect, useState } from 'react';
import SelectPin from './components/select-pin';
import { Pin, readPins } from './firebase'; // Import the functions from your Firebase module

interface Pins {
  inputPins: Pin[];
  outputPins: Pin[];
  analogPins: Pin[];
}

const App = () => {
  const [pins, setPins] = useState<Pins>({
    inputPins: [],
    outputPins: [],
    analogPins: [],
  });

  useEffect(() => {
    const pinIds = [16, 17, 18, 19, 32];
    readPins(pinIds, (updatedPins: Pin[]) => {
      const sortedPins: Pins = {
        inputPins: [],
        outputPins: [],
        analogPins: [],
      };

      updatedPins.filter((pin) => {
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
    <section className='flex items-center justify-center h-screen'>
      <div className='flex flex-col gap-4'>
        <SelectPin pins={pins.outputPins} type='output'></SelectPin>
        <SelectPin pins={pins.inputPins} type='input'></SelectPin>
        <SelectPin pins={pins.analogPins} type='analog'></SelectPin>
      </div>
    </section>
  );
};

export default App;
