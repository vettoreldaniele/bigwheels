RWByteAddressBuffer buf : register(u0);

[numthreads(1, 1, 1)] void csmain() {
    uint old = buf.Load(0u);
    buf.Store(0u, 1 + old);
}
