import { initializeApp } from 'firebase/app';
import { getDatabase, onValue, ref, set } from 'firebase/database';

export interface Pin {
  id: number;
  type: string;
  value: number;
}

const firebaseConfig = {
  apiKey: import.meta.env.VITE_FIREBASE_API_KEY,
  authDomain: import.meta.env.VITE_FIREBASE_AUTH_DOMAIN,
  databaseURL: import.meta.env.VITE_FIREBASE_DATABASE_URL,
  projectId: import.meta.env.VITE_FIREBASE_PROJECT_ID,
  storageBucket: import.meta.env.VITE_FIREBASE_STORAGE_BUCKET,
  messagingSenderId: import.meta.env.VITE_FIREBASE_MESSAGING_SENDER_ID,
  appId: import.meta.env.VITE_FIREBASE_APP_ID,
};

initializeApp(firebaseConfig);

const db = getDatabase();

export const setPin = ({ id, type, value }: Pin) => {
  const reference = ref(db, 'pins/' + id + '/');

  set(reference, {
    type,
    value,
  });
};

export const readPins = (ids: number[], callback: (pins: Pin[]) => void) => {
  const pins: Pin[] = [];
  ids.forEach((id) => {
    const reference = ref(db, 'pins/' + id + '/');

    onValue(reference, (snapshot) => {
      const data = snapshot.val();
      const pin: Pin = { id, ...data };

      const index = pins.findIndex((p) => p.id === id);
      if (index !== -1) {
        pins[index] = pin;
      } else {
        pins.push(pin);
      }
      callback([...pins]);
    });
  });
};
