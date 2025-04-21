'use client';

import { useEffect, useState } from 'react';
import { db } from '../../lib/firebase';
import { ref, onValue } from 'firebase/database';

export default function Home() {
  const [plantData, setPlantData] = useState<any>(null);

  useEffect(() => {

    const plantRef = ref(db, 'plant/');
    const unsubscribe = onValue(plantRef, (snapshot) => {
      const data = snapshot.val();
      setPlantData(data);
    });

    return () => unsubscribe();
  }, []);


  const formatTimestamp = (timestamp: number) => {
    if (!timestamp) return 'Unknown';
    const date = new Date(timestamp * 1000);
    return date.toLocaleString();
  };

  return (
    <main className="flex flex-col items-center justify-center min-h-screen bg-green-100 p-8">
      <h1 className="text-3xl text-gray-950 font-bold mb-4">🌱 Smart Plant Doctor</h1>
      {plantData ? (
        <div className="bg-white shadow-lg p-6 rounded-xl w-full max-w-lg">
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mb-4">
            <div className="bg-green-50 p-4 rounded-lg">
              <h2 className="text-xl text-gray-950 font-semibold mb-2"> Temperature</h2>
              <p className="text-2xl text-green-600">{plantData.temperature?.toFixed(1)}°C</p>
              <p className="text-sm text-gray-600 mt-1">{plantData.temperatureStatus}</p>
            </div>
            
            <div className="bg-blue-50 p-4 rounded-lg">
              <h2 className="text-xl text-gray-950 font-semibold mb-2"> Soil Moisture</h2>
              <p className="text-2xl text-blue-600">{plantData.soilMoisture}%</p>
              <p className="text-sm text-gray-600 mt-1">{plantData.moistureStatus}</p>
            </div>
            
            <div className="bg-yellow-50 p-4 rounded-lg">
              <h2 className="text-xl text-gray-950 font-semibold mb-2"> Light Level</h2>
              <p className="text-2xl text-yellow-600">{plantData.light?.toFixed(1)} lux</p>
              <p className="text-sm text-gray-600 mt-1">{plantData.lightStatus}</p>
            </div>
            
            <div className="bg-gray-50 p-4 rounded-lg">
              <h2 className="text-xl text-gray-950 font-semibold mb-2"> Plant Status</h2>
              <p className="text-sm text-gray-950">
                {plantData.soilMoisture < 30 && " Plant needs water!"}
                {plantData.soilMoisture > 80 && " Soil too wet!"}
                {plantData.soilMoisture >= 30 && plantData.soilMoisture <= 80 && " Soil moisture optimal"}
              </p>
              <p className="text-sm text-gray-950 mt-1">
                {(plantData.light < 150) ? " Insufficient light" : " Light level good"}
              </p>
            </div>
          </div>
          
          
        </div>
      ) : (
        <div className="bg-white shadow-lg p-6 rounded-xl w-full max-w-md text-center">
          <p className="text-gray-600">Connecting to your plant...</p>
          <div className="mt-4 w-8 h-8 border-t-2 border-green-500 border-solid rounded-full animate-spin mx-auto"></div>
        </div>
      )}
    </main>
  );
}
