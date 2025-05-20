# Abordagem com dicionário e memoização
memo = {}

def calcular_probabilidade(i, P, premios, probabilidades):
    if P <= 0:
        return 1.0
    if i == 0:
        return 0.0
    if (i, P) in memo:
        return memo[(i, P)]

    
    # Contribuição do cliente i (visitado ou não)
    p_i = premios[i-1]
    pi_i = probabilidades[i-1]
    
    # Caso 1: Cliente i é visitado
    prob_visitado = calcular_probabilidade(i-1, P - p_i, premios, probabilidades)
    
    # Caso 2: Cliente i não é visitado
    prob_nao_visitado = calcular_probabilidade(i-1, P, premios, probabilidades)
    
    resultado = pi_i * prob_visitado + (1 - pi_i) * prob_nao_visitado
    memo[(i, P)] = resultado
    return memo[(i, P)]

# Uso:
premios = [20, 100, 60]
probabilidades = [0.8, 0.2, 0.4]
P_min = 100
prob = calcular_probabilidade(3, P_min, premios, probabilidades)
print(f"Probabilidade: {prob:.2f}")  # Saída: 0.20