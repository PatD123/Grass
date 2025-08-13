Grass Simulation in OpenGL

## TODO
- [X] MORE Parallelizable MORE VECTORIZABLE!!!!
- 🤯 Learned to use AVX Intel Intrinsics.
- **At ~50k blades, SIMD sits consistently at high 30s - low 40s FPS. Scalar approach sits at mid 30s FPS.**
- **At ~125k blades, SIMD sits consistently at high 20s FPS. Scalar approach sinks to ~10 FPS. ▶️ Very good.**
- [X] Multithread when frustum culling, then draw only the ones that are not culled out
- :question: Apparently not great and doens't do as well as non-threaded.
- [ ] Frustum culling is currently done per-blade. Probably should be altered to cull per tile.

## The journey

With more and more grass, even with LODing it gets pretty slow; Implemented Frustum Culling. 
With good LODing and Frustum Culling, we get 60FPS.

https://github.com/user-attachments/assets/360df1e2-c34b-42c0-8e26-14a5eb0a804d

LODing helps to increase FPS, but of course at the cost of aesthetics.

<img width="796" height="624" alt="image" src="https://github.com/user-attachments/assets/24f8cd0c-5fff-4664-bd64-6525ec509d47" />


Look at that FPS!

<img width="784" height="627" alt="image" src="https://github.com/user-attachments/assets/0dcdc7d3-53de-4053-878a-8a04488e27b9" />


Beautiful Gaggle of Grass

<img width="756" height="463" alt="image" src="https://github.com/user-attachments/assets/eece504a-3297-4e76-887e-dfacc70a7a4b" />


<img width="775" height="570" alt="image" src="https://github.com/user-attachments/assets/4390c7b0-3436-4388-a6da-37f000f8eefa" />

Colorful Gaggle of Grass

<img width="629" height="461" alt="image" src="https://github.com/user-attachments/assets/37e1030d-b306-403f-bc5a-3ea6427ab558" />



Gaggle of Grass

<img width="579" height="532" alt="image" src="https://github.com/user-attachments/assets/23fc1088-4817-4820-aa6c-014985a6c773" />


Basic grass flaco

<img width="516" height="442" alt="image" src="https://github.com/user-attachments/assets/44be86a6-006d-4419-8c34-bfa0870468d4" />


Basic grass phat

<img width="373" height="327" alt="image" src="https://github.com/user-attachments/assets/f9e93241-066f-43d0-a1cf-1d3779478868" />
