package org.ilyadubinsky.cfpp.emv;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.ilyadubinsky.cfpp.utils.BitOps;
import org.ilyadubinsky.cfpp.utils.IO;

import lombok.NonNull;

//TODO: a reasonable mechanism to load and instantiate this table

public class CertificateAuthorityKeyTable {
	
	private static CertificateAuthorityKeyTable globalTable;
	
	public static CertificateAuthorityKeyTable getCAKeyTable() {
		return globalTable; }

	public static int [] ca_pk_1  = { 0xc3, 0x39, 0xcd, 0x81, 0x3e, 0xbe,
			0xab, 0xaf, 0xd4, 0xcb, 0x37, 0x11, 0x06, 0xb0, 0x03, 0x3e, 0x96, 0x72,
			0x03, 0x79, 0x93, 0x71, 0x55, 0xd8, 0x33, 0x37, 0x5d, 0x83, 0x3f, 0x00,
			0x2b, 0xc6, 0x0f, 0xa3, 0x7e, 0x18, 0xd5, 0xdd, 0x7b, 0x88, 0x02, 0xa1,
			0xf5, 0x8f, 0x4d, 0x62, 0xc6, 0x36, 0x7a, 0x1f, 0x3a, 0x9c, 0x0b, 0xa4,
			0xbb, 0x00, 0x57, 0xf5, 0xf3, 0x3e, 0x89, 0x14, 0xb4, 0xf9, 0x0e, 0xb7,
			0x56, 0x85, 0xe5, 0xbf, 0x8e, 0x17, 0x33, 0x5a, 0x55, 0x68, 0xbc, 0x18,
			0xa8, 0x72, 0x0d, 0xa3, 0x3c, 0x8e, 0x79, 0x4c, 0xe1, 0x4f, 0xb3, 0xa3,
			0xe7, 0xd5, 0xd0, 0x5e, 0x58, 0xf9, 0x00, 0x62, 0x16, 0xb0, 0xd8, 0xf8,
			0x4a, 0x06, 0x54, 0xfb, 0x9c, 0x10, 0x09, 0xd6, 0xe7, 0xb0, 0x6a, 0xe6,
			0xeb, 0xb4, 0x9a, 0xd4, 0xb1, 0x37, 0x55, 0x5b, 0x0b, 0x04, 0x8f, 0xc8,
			0xdc, 0x99, 0x23, 0x9c, 0x7f, 0x4c, 0x22, 0x5c, 0xce, 0xbf, 0xb7, 0x66,
			0x7f, 0xd9, 0x09, 0x10, 0x43, 0x7c, 0x7f, 0xe8, 0xe7, 0xae, 0x83, 0x15,
			0x75, 0x1a, 0xeb, 0xa6, 0xab, 0xd6, 0xed, 0x35, 0x74, 0x91, 0x7b, 0xb8,
			0xf6, 0x52, 0x6b, 0xbb, 0xfb, 0x30, 0x60, 0x73, 0xaa, 0x49, 0x3a, 0x18,
			0xff, 0x04, 0x6f, 0xf9, 0x9e, 0xff, 0xfe, 0x13, 0x42, 0x67, 0x65, 0xf4,
			0x9b, 0x80, 0xf1, 0x34, 0x6f, 0xbc, 0xd7, 0xd6, 0x6b, 0x1c, 0x44, 0x86,
			0x49, 0x72, 0x2c, 0xed, 0x53, 0xe8, 0x66, 0xc6, 0x82, 0xe3, 0x68, 0x1d,
			0x98, 0xc8, 0xf2, 0x9e, 0xff, 0xcd, 0x29, 0x84, 0x24, 0x53, 0xf2, 0x60,
			0x81, 0xb3, 0xa4, 0xb0, 0xd3, 0xd2, 0x8f, 0x18, 0x3a, 0x31, 0xc9, 0x7e,
			0x3d, 0x79, 0xbd, 0x49, 0x68, 0xcb, 0xca, 0x17, 0xfc, 0xfd, 0x1d, 0x5f,
			0x2f, 0xab };
	
	public static int[] ca_pk_2 = { 0x99, 0x6A, 0xF5, 0x6F, 0x56, 0x91, 0x87,
		0xD0, 0x92, 0x93, 0xC1, 0x48, 0x10, 0x45, 0x0E, 0xD8, 0xEE, 0x33, 0x57,
		0x39, 0x7B, 0x18, 0xA2, 0x45, 0x8E, 0xFA, 0xA9, 0x2D, 0xA3, 0xB6, 0xDF,
		0x65, 0x14, 0xEC, 0x06, 0x01, 0x95, 0x31, 0x8F, 0xD4, 0x3B, 0xE9, 0xB8,
		0xF0, 0xCC, 0x66, 0x9E, 0x3F, 0x84, 0x40, 0x57, 0xCB, 0xDD, 0xF8, 0xBD,
		0xA1, 0x91, 0xBB, 0x64, 0x47, 0x3B, 0xC8, 0xDC, 0x9A, 0x73, 0x0D, 0xB8,
		0xF6, 0xB4, 0xED, 0xE3, 0x92, 0x41, 0x86, 0xFF, 0xD9, 0xB8, 0xC7, 0x73,
		0x57, 0x89, 0xC2, 0x3A, 0x36, 0xBA, 0x0B, 0x8A, 0xF6, 0x53, 0x72, 0xEB,
		0x57, 0xEA, 0x5D, 0x89, 0xE7, 0xD1, 0x4E, 0x9C, 0x7B, 0x6B, 0x55, 0x74,
		0x60, 0xF1, 0x08, 0x85, 0xDA, 0x16, 0xAC, 0x92, 0x3F, 0x15, 0xAF, 0x37,
		0x58, 0xF0, 0xF0, 0x3E, 0xBD, 0x3C, 0x5C, 0x2C, 0x94, 0x9C, 0xBA, 0x30,
		0x6D, 0xB4, 0x4E, 0x6A, 0x2C, 0x07, 0x6C, 0x5F, 0x67, 0xE2, 0x81, 0xD7,
		0xEF, 0x56, 0x78, 0x5D, 0xC4, 0xD7, 0x59, 0x45, 0xE4, 0x91, 0xF0, 0x19,
		0x18, 0x80, 0x0A, 0x9E, 0x2D, 0xC6, 0x6F, 0x60, 0x08, 0x05, 0x66, 0xCE,
		0x0D, 0xAF, 0x8D, 0x17, 0xEA, 0xD4, 0x6A, 0xD8, 0xE3, 0x0A, 0x24, 0x7C,
		0x9F };
	

	static {
		
		CertificateAuthorityKey cak = new CertificateAuthorityKey();
		globalTable = new CertificateAuthorityKeyTable();
		
		cak.setAuthorityName("Visa");
		cak.setIndex((byte) 0x01);
		cak.setModulus(BitOps.toByteArray(ca_pk_1));
		cak.setPublicExponent(0x03);
		
		globalTable.addCA(cak);

		cak = new CertificateAuthorityKey();
		cak.setAuthorityName("MC");
		cak.setIndex((byte) 0x92);
		cak.setModulus(BitOps.toByteArray(ca_pk_2));
		cak.setPublicExponent(0x03);

		globalTable.addCA(cak);

	}

	private Map<Byte, CertificateAuthorityKey> table;
	
	public CertificateAuthorityKeyTable() {
		table = new HashMap<>();
	}
	
	void addCA(@NonNull CertificateAuthorityKey cak) {
		table.put(cak.getIndex(), cak);
	}
	
	public CertificateAuthorityKey getCA(byte index) {
		return table.get(index);
	}

	@Override
	public String toString() {
		Byte[] keys = (Byte[]) table.keySet().toArray(new Byte[] {});
		Arrays.sort(keys);
		
		StringBuffer buffer = new StringBuffer();
		
		for (Byte b : keys ) {
			buffer.append( IO.SEPARATOR ).append('\n');
			buffer.append(getCA(b).toString());
		}
		
		buffer.append('\n').append( IO.SEPARATOR );
		return buffer.toString();
	}
	
	
}
