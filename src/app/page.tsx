'use client';

import { useEffect, useState } from 'react';
import { db } from '../../lib/firebase';
import { ref, onValue } from 'firebase/database';

export default function Home() {
  const [sensorData, setSensorData] = useState<any>(null);

  useEffect(() => {
    const sensorRef = ref(db, 'sensorData/');
    const unsubscribe = onValue(sensorRef, (snapshot) => {
      const data = snapshot.val();
      setSensorData(data);
    });

    return () => unsubscribe();
  }, []);

  return (
    <main className="flex flex-col items-center justify-center min-h-screen bg-green-100 p-8">
      <h1 className="text-3xl font-bold mb-4">🌱 Live Plant Monitor</h1>
      {sensorData ? (
        <div className="bg-white shadow-lg p-6 rounded-xl w-full max-w-md text-center">
          <p>🌡️ Temperature: {sensorData.temperature}°C</p>
          <p>💧 Moisture: {sensorData.moisture}%</p>
          <p>💨 Humidity: {sensorData.humidity}%</p>
        </div>
      ) : (
        <p className="text-gray-600">Fetching data...</p>
      )}
    </main>
  );
}
