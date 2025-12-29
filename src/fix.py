import os

def fix_ppm_red_to_black(input_file, output_file):
    print(f"Traitement de {input_file}...")
    
    with open(input_file, 'rb') as f_in, open(output_file, 'wb') as f_out:
        # 1. Copier l'entête (P6, dimensions, 255)
        # On lit les 3 premières lignes textuelles
        for _ in range(3):
            line = f_in.readline()
            f_out.write(line)
        
        # 2. Traiter les pixels (RVB : 3 octets par pixel)
        # On définit la couleur cible (rouge pur) et la nouvelle (noir)
        target_red = b'\x00\x00\x00'
        replacement_black = b'\xff\x00\x00'
        
        count = 0
        while True:
            # On lit par paquets de 3 octets (1 pixel)
            pixel = f_in.read(3)
            if not pixel:
                break # Fin du fichier
            
            if pixel == target_red:
                f_out.write(replacement_black)
                count += 1
            else:
                f_out.write(pixel)
                
    print(f"Terminé ! {count} pixels rouges ont été remplacés.")
    print(f"Nouveau fichier sauvegardé sous : {output_file}")

# Utilisation
input_name = "assets/skybox2.ppm" # Mets le nom de ton fichier 8K ici
output_name = "assets/skybox3.ppm"
fix_ppm_red_to_black(input_name, output_name)