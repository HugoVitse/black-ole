
from sympy.printing.str import StrPrinter
from sympy.printing.precedence import precedence
from sympy import symbols, Matrix, diff, sin, cos, simplify

r, theta, phi, t = symbols('x.x x.y x.z x.w')
kr, ktheta, kphi, kt = symbols('k.x k.y k.z k.w')

coords = [r, theta, phi,t]
kcoords = [kr,ktheta,kphi,kt]


# vibe coded shit to transform ** operator to classic multiplication
class UnrollPowPrinter(StrPrinter):
    def _print_Pow(self, expr):
        # Si l'exposant est un entier positif (ex: 2, 3...)
        if expr.exp.is_Integer and expr.exp > 0:
            # On récupère la représentation de la base (ex: "x" ou "(x+y)")
            # parenthesize assure qu'on met des () si nécessaire autour de la base
            base_str = self.parenthesize(expr.base, precedence(expr))
            
            # On répète la base autant de fois que l'exposant
            return "*".join([base_str] * int(expr.exp))
            
        # Sinon (ex: x**-1 ou x**0.5), on garde le comportement normal
        return super()._print_Pow(expr)


def print_unroll(expr):
    printer = UnrollPowPrinter()
    print(printer.doprint(expr))



#metric
rs, m , moment= symbols('rs mass moment')

alpha, delta, epsilon = symbols('alpha delta epsilon')


def chooseMetric(type):
    if(type == "SCH"):
        return Matrix([
            [ (1-rs/r)**(-1), 0,    0,                       0           ], # r
            [ 0,              r**2, 0,                       0           ], # theta
            [ 0,              0,    (r**2)*sin(theta)**2,    0           ], # phi
            [ 0,              0,    0,                       -(1-rs/r)   ]  # t
        ])
    if(type == "KERR"):
        return Matrix([
            [ delta/epsilon,  0,     0,                                                                 0                                     ], # r
            [ 0,              delta, 0,                                                                 0                                     ], # theta
            [ 0,              0,     (alpha + (rs*r*(moment**2)*sin(theta)**2)/delta)*sin(theta)**2,    -(rs*r*moment*sin(theta)**2)/delta    ], # phi
            [ 0,              0,     -(rs*moment*r*sin(theta)**2)/delta,                                -(1 - (rs*r)/delta)                   ]  # t
        ])


rs = 2*m


alpha = r**2 + moment**2
delta =  r**2 + (moment**2)*(cos(theta)**2)
epsilon = r**2 + moment**2 - rs*r

metric = chooseMetric("SCH")



g_inv = metric.inv()


n = 4
gamma = [[[0 for _ in range(n)] for _ in range(n)] for _ in range(n)]

for mu in range(n):
    for alpha in range(n):
        for beta in range(n):
            
            somme = 0
            for l in range(n):
                derivees = (diff(metric[l, alpha], coords[beta]) +
                            diff(metric[l, beta], coords[alpha]) -
                            diff(metric[alpha, beta], coords[l]))
                
                somme += 0.5 * g_inv[mu, l] * derivees
            
            valeur = simplify(somme)
            
            if valeur != 0:
                gamma[mu][alpha][beta] = valeur
              




dk = [0,0,0,0]
for mu in range(n):
    dk[mu] = 0;
    for alpha in range(n):
        for beta in range(n):
            chr = gamma[mu][alpha][beta]
            dk[mu] +=  chr*kcoords[alpha]*kcoords[beta]
            print(chr)

    dk[mu] = simplify(-dk[mu])


printer = UnrollPowPrinter()
dk_str = [printer.doprint(term) for term in dk]


f_func = f"""

struct Photon {{
    vec4 x; // r, theta, phi, t
    vec4 k; // kr, ktheta, kphi, kt
}};

Photon F(inout vec4 x, inout vec4 k, inout BlackHole blackhole) {{


    Photon p;
    p.x = vec4(k.x, k.y, k.z, k.w);    // dx/dt = k
    p.k = vec4({dk_str[0]}, {dk_str[1]}, {dk_str[2]}, {dk_str[3]}); // dk/dt = accélération
    return p;
}}


void RK4step(inout BlackHole blackhole, float pas, inout Photon photon) {{

    Photon p = Photon(photon.x, photon.k);
    Photon k1 = F(p.x, p.k, blackhole);
    
    Photon p2 = Photon(photon.x + k1.x * (pas*0.5), photon.k + k1.k * (pas*0.5));
    Photon k2 = F(p2.x, p2.k, blackhole);
    
    Photon p3 = Photon(photon.x + k2.x * (pas*0.5), photon.k + k2.k * (pas*0.5));
    Photon k3 = F(p3.x, p3.k, blackhole);
    
    Photon p4 = Photon(photon.x + k3.x * pas, photon.k + k3.k * pas);
    Photon k4 = F(p4.x, p4.k, blackhole);


    photon.x += (k1.x + k2.x*2.0 + k3.x*2.0 + k4.x) * (pas/6.0);
    photon.k += (k1.k + k2.k*2.0 + k3.k*2.0 + k4.k) * (pas/6.0);
}}

"""

print(f_func)

f = open("photon.glsl", "w")
f.write(f_func)
