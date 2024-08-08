import { useState } from 'react';
import { Pin, setPin } from '../firebase';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from './ui/select';

import { Toggle } from './ui/toggle';

interface SelectPinProps {
  pins: Pin[];
  type: string;
}

const SelectPin = ({ pins, type }: SelectPinProps) => {
  const [selectedPin, setSelectedPin] = useState<Pin | null>(null);

  const handleValueChange = (value: string) => {
    const pinId = Number(value);
    const selectedPin = pins.find((pin) => pin.id === pinId);
    if (selectedPin) setSelectedPin(selectedPin);
  };

  const changePinValue = () => {
    if (selectedPin === null) return;
    const newValue = selectedPin.value === 0 ? 1 : 0;
    const newPin: Pin = {
      id: selectedPin.id,
      type: selectedPin.type,
      value: newValue,
    };

    setSelectedPin(newPin);
    setPin(newPin);
  };

  return (
    <>
      <Select
        onValueChange={handleValueChange}
        value={selectedPin?.id.toString()}
      >
        <SelectTrigger className='w-[180px]'>
          <SelectValue
            placeholder={type.charAt(0).toUpperCase() + type.slice(1)}
          />
        </SelectTrigger>
        <SelectContent>
          {pins.map(({ id }) => {
            return <SelectItem value={id.toString()}>{`Pin ${id}`}</SelectItem>;
          })}
        </SelectContent>
      </Select>
      {type === 'output' ? (
        <Toggle onClick={changePinValue}>
          {selectedPin?.value === 0 ? 'off' : 'on'}
        </Toggle>
      ) : (
        <p>{selectedPin?.value}</p>
      )}
    </>
  );
};

export default SelectPin;
