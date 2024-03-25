void lcg_seed(unsigned int seed) {
    lcg_state = seed;
}

unsigned int lcg_rand() {
    lcg_state = (LCG_MULTIPLIER * lcg_state + LCG_INCREMENT) % LCG_MODULUS;
    return lcg_state;
}