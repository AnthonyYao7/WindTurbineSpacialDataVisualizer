import React, { useEffect, useState } from 'react';
import { MapContainer, TileLayer, useMap } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet-draw/dist/leaflet.draw.css';
import 'leaflet-draw';
import axios from 'axios';
import leafletPip from 'leaflet-pip';

function MyMap() {
    const [drawControl, setDrawControl] = useState(null);

    const MyComponent = () => {
        const map = useMap();
        console.log("hi")
        useEffect(() => {
            if (!drawControl) {
                const control = new L.Control.Draw({
                    draw: {
                        polyline: false,
                        polygon: false,
                        circle: false,
                        marker: false,
                        rectangle: true,
                    },
                });
                setDrawControl(control);
                map.addControl(control);
            }
        }, [map, drawControl]);

        useEffect(() => {
            axios.get(process.env.PUBLIC_URL + '/data/UnitedStatesPolygon.geo.json')
                .then(res => {
                    const geoJsonLayer = L.geoJSON(res.data);
                    geoJsonLayer.addTo(map);

                    map.on(L.Draw.Event.CREATED, function (e) {
                        const type = e.layerType;
                        const layer = e.layer;

                        if (type === 'rectangle') {
                            const latlngs = layer.getLatLngs();
                            console.log(`${type} corners: `, latlngs);

                            latlngs[0].forEach(point => {

                                const result = leafletPip.pointInLayer([point.lng, point.lat], geoJsonLayer, false);

                                if(result.length === 0) {
                                    console.log("Point is outside US: ", point);
                                }
                                else {
                                    console.log("Point is inside US: ", point);
                                }
                            });
                        }
                    });
                });
        }, [map]);

        return null;
    };

    return (
        <MapContainer
            center={[39.50, -98.35]}
            zoom={4}
            style={{ height: "100vh", width: "100%" }}
        >
            <TileLayer
                attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            />
            <MyComponent />
        </MapContainer>
    );
}

export default MyMap;
