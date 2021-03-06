//  Copyright (C) 2010-2012 VSTU
//
//	This file is part of Isilme SDK.
//
//		Isilme SDK is free software: you can redistribute it and/or modify
//		it under the terms of the GNU Lesser General Public License as published by
//		the Free Software Foundation, either version 3 of the License, or
//		(at your option) any later version.
//
//		Isilme SDK is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU Lesser General Public License for more details.
//
//		You should have received a copy of the GNU Lesser General Public License
//		along with Isilme SDK.  If not, see <http://www.gnu.org/licenses/>.
//
//	���� ���� � ����� Isilme SDK.
//
//		Isilme SDK - ��������� ���������: �� ������ ������������������ �� �/���
//		�������� �� �� �������� ������� ����������� ������������ �������� GNU � ��� ����,
//		� ����� ��� ���� ������������ ������ ���������� ������������ �����������;
//		���� ������ 3 ��������, ���� (�� ������ ������) ����� ����� �������
//		������.
//
//		Isilme SDK ���������������� � �������, ��� ��� ����� ��������,
//		�� ���� ������ ��������; ���� ��� ������� �������� ��������� ����
//		��� ����������� ��� ������������ �����. ��������� ��. � ������� �����������
//		������������ �������� GNU.
//
//		�� ������ ���� �������� ����� ������� ����������� ������������ �������� GNU
//		������ � ���� ����������. ���� ��� �� ���, ��.
//		<http://www.gnu.org/licenses/>.
#include "IsilmePCH.h"

// BodyDef

BodyDef::BodyDef() :
	Type(BodyDef::SimpleBody)
{
}

void			BodyDef::ParseB2DBody(TiXmlElement* bodyElement)
{
	// ������� ����������� ����
	int isBullet		= 0;
	float angle			= 0;
	int isAllowSleep	= 1;
	int isFixedRotation	= 0;
	int isActive		= 1;

	// ��������� ��� ����
	if (strcmp(bodyElement->Attribute("Type"), "Dynamic") == 0)
		B2DBodyDef.type = b2_dynamicBody;
	else if (strcmp(bodyElement->Attribute("Type"), "Kinematic") == 0)
		B2DBodyDef.type = b2_kinematicBody;
	else
		B2DBodyDef.type = b2_staticBody;

	// ����������� ��������� ����
	bodyElement->QueryIntAttribute("Bullet", &isBullet);
	bodyElement->QueryIntAttribute("FixedRotation", &isFixedRotation);
	bodyElement->QueryFloatAttribute("Angle", &angle);
	bodyElement->QueryIntAttribute("AllowSleep", &isAllowSleep);
	bodyElement->QueryIntAttribute("Active", &isActive);

	// ������������� ��������� ����
	B2DBodyDef.bullet			= (isBullet != 0);
	B2DBodyDef.angle			= angle;
	B2DBodyDef.allowSleep		= (isAllowSleep != 0);
	B2DBodyDef.fixedRotation	= (isFixedRotation != 0);
	B2DBodyDef.active			= (isActive != 0);
	B2DBodyDef.linearDamping	= 0.0f;
	B2DBodyDef.angularDamping	= 0.1f;
	
	bodyElement->QueryFloatAttribute("LinearDumping", &(B2DBodyDef.linearDamping));
	bodyElement->QueryFloatAttribute("AngularDumping", &(B2DBodyDef.angularDamping));

	// ������� ��������� ����
	TiXmlElement* fixtureElement = bodyElement->FirstChildElement();
	for (fixtureElement; fixtureElement; fixtureElement = fixtureElement->NextSiblingElement())
	{
		b2FixtureDef* fixtureDef = ParseB2DFixture(fixtureElement);
		B2DFixtures.push_back(fixtureDef);
	}
}

b2FixtureDef*	BodyDef::ParseB2DFixture(TiXmlElement* fixtureElement)
{
			
	// ������� ������
	b2FixtureDef* fixtureDef = new b2FixtureDef();
	b2CircleShape circleShape;
	float friction	= 0.3f;
	float density	= 1.0f;
	int maskBits	= 0;
	int categoryBits = 0;
	int isSensor	= 0;
	float restitution = 0.1f;

	// ������ ��������� ������
	fixtureElement->QueryFloatAttribute("Friction", &friction);
	fixtureElement->QueryFloatAttribute("Density", &density);
	fixtureElement->QueryIntAttribute("MaskBits", &maskBits);
	fixtureElement->QueryIntAttribute("CategoryBits", &categoryBits);
	fixtureElement->QueryIntAttribute("Sensor", &isSensor);
	fixtureElement->QueryFloatAttribute("Restitution", &restitution);

	// ��������� ���������
	fixtureDef->restitution = restitution;
	fixtureDef->density = density;
	fixtureDef->isSensor = (isSensor != 0);
	fixtureDef->friction = friction;
	fixtureDef->filter.groupIndex = 0;
	fixtureDef->filter.maskBits = maskBits;
	fixtureDef->filter.categoryBits = categoryBits;

	// ������ ���������
	if (strcmp(fixtureElement->Value(), "Box") == 0)
	{
		b2PolygonShape* poligonShape = new b2PolygonShape();
		// ������� �������������
		float width = 1.0f;
		float height = 1.0f;
		fixtureElement->QueryFloatAttribute("Width", &width);
		fixtureElement->QueryFloatAttribute("Height", &height);
		poligonShape->SetAsBox(width / 2.0f, height / 2.0f);
		fixtureDef->shape = poligonShape;
		
	}
	else if (strcmp(fixtureElement->Value(), "Circle") == 0)
	{
		b2CircleShape* circleShape = new b2CircleShape();
		// ������� ����������
		float radius = 1.0f;
		fixtureElement->QueryFloatAttribute("Radius", &radius);
		circleShape->m_radius = radius;
		fixtureDef->shape = circleShape;
	}
	else if ((strcmp(fixtureElement->Value(), "Polygon")) == 0)
	{
		// ������� �������������
	}
		
	return fixtureDef;
}

void	BodyDef::Parse(TiXmlElement* bodyElement)
{
	if (bodyElement == 0)
	{
		Type = BodyDef::SimpleBody;
	}
	else
	{
		Type = BodyDef::Box2DBody;
		ParseB2DBody(bodyElement);
	}
}

// Body

Body::~Body()
{
}

float Body::GetScale()
{
	return mScale;
}

void Body::SetScale(float value)
{
	float oldScale = mScale;
	mScale = value;
	OnScale(oldScale, mScale);
}

void Body::Scale(float value)
{
	SetScale(mScale * value);
}

void Body::OnScale(float oldScale, float scale)
{

}

Body::Body()
{
	mScale = 1.0f;
}

BodyDef::BodyType Body::GetType()
{
	return mType;
}

void	Body::SetActive(bool flag)
{
}

// Box2dBody

Box2DBody::Box2DBody(BodyDef* def) 
{
	mOfflineAngle = 0.0f;
	mOfflinePosition.Set(0.0f, 0.0f);
	// ��������� ���
	mType = BodyDef::Box2DBody;
	mBody = 0;
	mEntity = 0;
	mBodyDefinition = def;
}

Box2DBody::~Box2DBody()
{
	// ���������� Box2D-����
	if (mBody)
	{
		mBody->GetWorld()->DestroyBody(mBody);
	}
}

void	Box2DBody::OnScale(float oldScale, float scale)
{
	if (mBody == 0)
		return;

	float k = scale / oldScale;

	int n = mBody->GetFixtureCount();
	b2Fixture** oldFixtures = (b2Fixture**)malloc(sizeof(b2Fixture*) * (n+5));
	for (int i = 0; i < n; ++i)
		oldFixtures[i] = mBody->GetFixtureList() + i;

	for (int i = 0; i < n; ++i)
	{
		b2Fixture* fixture = oldFixtures[i];
		b2FixtureDef def;
		def.filter = fixture->GetFilterData();
		def.friction = fixture->GetFriction();
		def.isSensor = fixture->IsSensor();
		def.restitution = fixture->GetRestitution();
		def.density = fixture->GetDensity();

		if (fixture->GetType() == b2Shape::e_circle)
		{
			b2CircleShape s;
			s.m_radius = fixture->GetShape()->m_radius * k;
			s.m_p = k * ((b2CircleShape*)fixture->GetShape())->m_p;
			def.shape = &s;
			mBody->CreateFixture(&def);
		}
		else if (fixture->GetType() == b2Shape::e_polygon)
		{
			b2PolygonShape s;
			s.m_centroid = k * ((b2PolygonShape*)fixture->GetShape())->m_centroid;
			s.m_vertexCount = ((b2PolygonShape*)fixture->GetShape())->m_vertexCount;
			for (int j = 0; j < s.m_vertexCount; ++j)
			{
				s.m_vertices[j] = k * ((b2PolygonShape*)fixture->GetShape())->m_vertices[j];
				s.m_normals[j] = ((b2PolygonShape*)fixture->GetShape())->m_normals[j];
			}

			def.shape = &s;
			mBody->CreateFixture(&def);
		}
		mBody->DestroyFixture(fixture);
	}
	free(oldFixtures);
}

void		Box2DBody::SetEntity(Entity* entity)
{
	mEntity = entity;
}

Entity*		Box2DBody::GetEntity()
{
	return (Entity*)(mBody->GetUserData());
}

b2Body*	Box2DBody::GetBox2DBody()
{
	return mBody;
}

Vector2& Box2DBody::GetPosition()
{
	if (mBody)
		return (Vector2&)(mBody->GetPosition());
	else
		return mOfflinePosition;
}

void Box2DBody::SetPosition(Vector2& position)
{
	if (mBody)
		mBody->SetTransform(position, mBody->GetAngle());
	else
		mOfflinePosition = position;
}

float Box2DBody::GetAngle()
{
	if (mBody)
		return mBody->GetAngle();
	else
		return mOfflineAngle;
}

void Box2DBody::SetAngle(float angle)
{
	if (mBody)
		mBody->SetTransform(mBody->GetPosition(), angle);
	else
		mOfflineAngle = angle;
}

void Box2DBody::SetActive(bool flag)
{
	mBody->SetActive(flag);
}

Vector2	Box2DBody::GetLinearVelocity()
{
	return mBody->GetLinearVelocity();
}

void	Box2DBody::SetLinearVelocity(Vector2& velocity)
{
	mBody->SetLinearVelocity(velocity);
}

Fixture*	Box2DBody::GetFixture(std::string& name)
{
	return mFixtures[name];
}

Fixture*	Box2DBody::CreateFixture(std::string& name, b2FixtureDef* fixtureDef)
{
	Fixture* fixture = mBody->CreateFixture(fixtureDef); 
	fixture->SetUserData((void*)(name.c_str()));
	mFixtures[name] = fixture;
	return fixture;
}

void	Box2DBody::DestroyFixture(std::string& name)
{
	Fixture* fixture = mFixtures[name];
	mBody->DestroyFixture(fixture);
}

void	Box2DBody::DestroyFixture(Fixture* fixture)
{
	std::string name = (const char*)(fixture->GetUserData());
	mBody->DestroyFixture(fixture);
	mFixtures.erase(name);
}

void	Box2DBody::AttachToLevel(LevelPtr level)
{
	b2World* world;

	// ���������� �� ������� ����
	if (mBody)
	{
		if (level)
		{
			if (mBody->GetWorld() == level->GetWorld())
			{
				// ���� �������� �������������� � ���� �� ������ ������, ���������� ��
				return;
			}
		}

		// ���������� Offline-����������
		mOfflinePosition = GetPosition();
		mOfflineAngle = GetAngle();

		world = mBody->GetWorld();
		world->DestroyBody(mBody);
		mBody = 0;
	}

	// ����������� � ������ ������
	if (level)
	{
		world = level->GetWorld();
		mBody = world->CreateBody(&(mBodyDefinition->B2DBodyDef));
		
		for (FixtureDefList::iterator it = mBodyDefinition->B2DFixtures.begin(); it != mBodyDefinition->B2DFixtures.end(); ++it)
		{
			mBody->CreateFixture(*it);
		}

		mBody->SetUserData((void*)mEntity);

		// ������������� ���������� � ������ ����
		SetPosition(mOfflinePosition);
		SetAngle(mOfflineAngle);
	}
	
}

// SimpleBody

SimpleBody::SimpleBody() :
	mPosition(0.0f, 0.0f),
	mAngle(0)
{
	mType = BodyDef::SimpleBody;
}

SimpleBody::~SimpleBody()
{
}

void SimpleBody::SetEntity(Entity* entity)
{
	mEntity = entity;
}

Entity* SimpleBody::GetEntity()
{
	return mEntity;
}

Vector2& SimpleBody::GetPosition()
{
	return mPosition;
}

float	SimpleBody::GetAngle()
{
	return mAngle;
}

void	SimpleBody::SetAngle(float angle)
{
	mAngle = angle;
}

void	SimpleBody::SetPosition(Vector2& position)
{
	mPosition = position;
}

void	SimpleBody::SetLinearVelocity(Vector2& velocity)
{
	
}

Vector2 SimpleBody::GetLinearVelocity()
{
	return Vector2(0.0f, 0.0f);
}

Fixture*	SimpleBody::GetFixture(std::string& fixture)
{
	return 0;
}

void	SimpleBody::AttachToLevel(LevelPtr level)
{
}